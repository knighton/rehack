class PossessiveDecider(object):
    """
    Determine whether the possessive form of a word should end with 's or '.
    """

    def apos_or_apos_s(self, word):
        if word.endswith('s'):
            return "'"
        else:
            return "'s"

