#pragma once

#include <Ciphey/candidates.hpp>

#include <memory>

namespace Ciphey {
  struct simple_analysis_res {
    freq_table freqs;
    prob_table probs;
    string_t val;
  };

  inline std::shared_ptr<const simple_analysis_res> analyse_string(string_t str) {
    auto ret = std::make_shared<simple_analysis_res>();
    ret->val = std::move(str);
    freq_analysis(ret->freqs, ret->val);
    freq_conv(ret->probs, ret->freqs);
    return ret;
  }

  inline Ciphey::crack_results<Ciphey::caesar::key_t> caesar_crack(std::shared_ptr<const simple_analysis_res> in,
                                                                   prob_table expected, group_t group,
                                                                   bool do_filter_missing = false, prob_t p_value = 1) {
    if (do_filter_missing) {
      prob_table tab = in->probs;
      filter_missing(tab, expected);
      return caesar::crack(std::move(tab), expected, group, in->val.length(), p_value);
    }

    return caesar::crack(in->probs, expected, group, in->val.length(), p_value);
  }

  inline string_t caesar_decrypt(string_t str, Ciphey::caesar::key_t key, group_t group) {
    caesar::decrypt(str, key, group);
    return str;
  }

  inline Ciphey::crack_results<Ciphey::vigenere::key_t> viginere_crack(string_t str,
                                                                       prob_table const& expected,
                                                                       group_t const& group, size_t key_length,
                                                                       bool do_filter_missing = false,
                                                                       prob_t p_value = 1) {
    windowed_freq_table freqs(key_length);
    freq_analysis(freqs, str);
    windowed_prob_table probs;
    probs.reserve(key_length);
    for (auto& i : freqs)
      freq_conv(probs.emplace_back(), i);

    if (do_filter_missing)
      for (auto& i : probs)
        filter_missing(i, expected);

    return vigenere::crack(probs, expected, group, str.length(), p_value);
  }
}