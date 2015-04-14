from py.contraction.bigram_contractor import BigramContractor


class Contractor(object):
    def __init__(self, bigram_contractor):
        self._bigrams = bigram_contractor

    def contract(self, tokens, only_required_by_syntax):
        ss = []
        i = 0
        while i + 1 < len(tokens):
            first = tokens[i]
            second = tokens[i + 1]
            s = self._bigrams.maybe_contract(
                first, second, only_required_by_syntax)
            if s:
                ss.append(s)
                i += 2
            else:
                ss.append(first.text())
                i += 1
        if i < len(tokens):
            assert i + 1 == len(tokens)
            tok = tokens[i]
            ss.append(tok.text())
        return ss
