# The token text to represent 's and ' as appropriate.
POSSESSIVE_MARK = '<POS>'


class Token(object):
    """
    A single token, with all the properties contracting et al need to work.
    """

    def __init__(self, text, is_nontransitive_verb, is_aux_verb):
        # The actual text.
        self._text = text

        # The verb words of nontransitive verbs are never contracted, except for
        # "not".
        # - "I am." not "I'm."
        # - "It's true because it is." not "it's".
        self._is_nontransitive_verb = is_nontransitive_verb

        # For example, "to have" contracts only if it's a transitive auxiliary
        # verb or auxiliary "to have" + "not".
        # - "Have you been there?"  "I have." not "I've.", but "I've been
        #   there." not "I have been there.".
        # - "I have a cat", not "I've a cat." (in US English, anyway).
        self._is_aux_verb = is_aux_verb

    def text(self):
        return self._text

    def is_possessive_mark(self):
        return self._text == POSSESSIVE_MARK

    def is_nontransitive_verb(self):
        return self._is_nontransitive_verb

    def is_aux_verb(self):
        return self._is_aux_verb
