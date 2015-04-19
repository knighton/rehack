#ifndef CC_VERB_INTERNAL_CONJUGATION_CONJUGATION_SPEC_H_
#define CC_VERB_INTERNAL_CONJUGATION_CONJUGATION_SPEC_H_

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

// -----------------------------------------------------------------------------

// The specification for how to conjugate a verb.
class ConjugationSpec {
  public:
    const string& lemma() const { return lemma_; }
    const string& pres_part() const { return pres_part_; }
    const string& past_part() const { return past_part_; }
    const vector<string>& nonpast() const { return nonpast_; }
    const vector<string>& past() const { return past_; }
    bool has_do_support() const { return has_do_support_; }

    void Init(const string& lemma, const string& pres_part,
              const string& past_part, const vector<string>& nonpast,
              const vector<string>& past);

    void AnnotateAsAux();

    // Field index means:
    // * 0    lemma
    // * 1    pres part
    // * 2    past part
    // * 3-8  nonpast (1S 2S 3S 1P 2P 3P)
    // * 9-15 past (1S 2S 3S 1P 2P 3P)
    const string& GetField(unsigned field_index) const;

    void ToString(string* s);
    void FromString(const string& s);

  private:
    // Basics.
    string lemma_;            // Lemma ("go").
    string pres_part_;        // Present participle ("going").
    string past_part_;        // Past participle ("gone").

    // Six nonpast ("go", "goes") and past ("went") for 3 persons x 2 numbers.
    //
    // Indicative mood only.  Subjunctive, imperative moods are handled using
    // indicative's fields..
    vector<string> nonpast_;
    vector<string> past_;

    // Do-support: whether to fall back to auxiliary "do" in negative or
    // interrogative clauses (eg "do you know?" vs "arent' you?").
    //
    // True for every verb except to_be.
    bool has_do_support_;
};

// -----------------------------------------------------------------------------

// All the conjugated words as one list, and indexes into it.
class FlatConjSpec {
  public:
    const vector<string>& words() const { return words_; }
    const map<string, vector<int> >& s2xx() const { return s2xx_; }

    void ToConjSpec(ConjugationSpec* spec);
    void FromConjSpec(const ConjugationSpec& spec);

  private:
    vector<string> words_;
    map<string, vector<int> > s2xx_;
};

// -----------------------------------------------------------------------------

class ConjugationSpecConfig {
  public:
    const vector<ConjugationSpec>& specs() const { return specs_; }

    void ToString(string* s);
    void FromString(const string& s);

  private:
    vector<ConjugationSpec> specs_;
};

// -----------------------------------------------------------------------------

#endif  // CC_VERB_INTERNAL_CONJUGATION_CONJUGATION_SPEC_H_
