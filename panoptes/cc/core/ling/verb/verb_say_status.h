#ifndef VERB_SAY_STATUS
#define VERB_SAY_STATUS

// OK is good.
// ERR means avoidable programming error.
// INVALID means bad (non-productive) verb configuration.
enum VerbSayStatus {
    VSS_OK = 0,

    // -------------------------------------------------------------------------
    // Deep converter.

    // Relative clauses can only contain finite verbs.
    VSS_INVALID_REL_CLAUSES_CAN_ONLY_CONTAIN_FINITE = 1,

    // -------------------------------------------------------------------------
    // Deep saying.

    // If you're gonna say it, verb must not have any unset fields.
    VSS_ERR_HAS_UNSET_FIELDS = 2,

    // Conflict: zero-relative pronoun relative clauses can drop words and front,
    // and pro-verbs only keep certain words in front.
    VSS_INVALID_RELATIVE_PRO_VERB_CONFLICT = 3,

    // You can only split finite verbs.
    VSS_INVALID_CAN_ONLY_SPLIT_FINITE = 4,

    // If non-finite, modality must be indicative.
    VSS_INVALID_IF_NON_FINITE_MODALITY_MUST_BE_INDICATIVE = 5,

    // -------------------------------------------------------------------------
    // Deep slice.

    // The given verb words to slice were empty.
    VSS_ERR_NO_VERB_WORDS = 6,

    // More than one "not" in the said verb words.
    VSS_ERR_MULTIPLE_NOTS = 7,

    // There is no conditional form of the modality you have chosen.
    VSS_INVALID_CONDITIONAL_FORM_OF_MODALITY_DNE = 8,

    // -------------------------------------------------------------------------
    // Surface converter.

    // You can't have both a modal verb and a non-indicative mood.
    VSS_INVALID_CANT_HAVE_BOTH_MODALS_AND_NON_IND = 9,

    // Imperatives are second person-only.
    VSS_INVALID_IMPERATIVES_ARE_2ND_PERSON = 10,

    // Partially mood-specific surface tense and mood must match.
    VSS_SURFACE_TENSE_NOT_OK_WITH_MOOD = 11,

    // Non-fintie vers are indicatve and don't have modals.
    VSS_INVALID_NON_FINITES_CANT_HAVE_MOODS_OR_MODALS = 12,

    // -------------------------------------------------------------------------
    // Surface saying.

    // No past tense form for the given modal.
    VSS_INVALID_MODAL_IS_UNKNOWN = 13
};

#endif  // VERB_SAY_STATUS
