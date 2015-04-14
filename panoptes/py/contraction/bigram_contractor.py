import re

from py.base.table import Table
from py.base.token import POSSESSIVE_MARK, Token
from py.possessive.possessive_renderer import PossessiveRenderer


class RegexListMatcher(object):
    def __init__(self, patterns):
        self.regexes = map(re.compile, patterns)

    def matches(self, s):
        for r in self.regexes:
            if r.search(s):
                return True
        return False


class BigramContractor(object):
    """
    Contracts bigrams.
    """

    def _extract_all_second_tokens(self):
        r = set()
        for (_, second) in self._mandatory_bigram2contraction:
            r.add(second)
        for (_, second) in self._bigram2contraction:
            r.add(second)
        for (_, second) in self._bigram2contract:
            r.add(second)
        for (_, second) in self._s_second2contract:
            r.add(second)
        return r

    def __init__(self, ends_with_s_detector, possessive_renderer,
                 mandatory_bigram2contraction, bigram2contraction,
                 bigram2contract, s_second2contract, second2suffix):
        # Whether a token's text ends with something that sounds vaguely like an
        # S.
        self._ends_with_s_detector = ends_with_s_detector

        # Whether to form the possessive by adding ' or 's.
        self._possessive_renderer = possessive_renderer

        # Even if not contracting anything else, it + 's = its.
        self._mandatory_bigram2contraction = mandatory_bigram2contraction

        # (first, second) -> contraction exception.
        self._bigram2contraction = bigram2contraction

        # (first, second) -> whether to contract normally.
        self._bigram2contract = bigram2contract

        # (whether first ended with s sound, second) -> whether to contract in
        # general (fallback from bigram2contract).
        self._s_second2contract = s_second2contract

        # Last token of bigram -> contraction suffix.
        self._second2suffix = second2suffix

        # If the second token in a pair is not in this set, the pair is not
        # going to be contracted.  For performance.
        self._seconds_set = self._extract_all_second_tokens()

    @staticmethod
    def init_default():
        ends_with_s_detector = RegexListMatcher("""
            s$
            x$
            z$
            ch$
            sh$
            zh$
            th$
            [iy][sz]ed?$
            [aeiourl][cs]e$
            [aeiourl]the$
        """.split())

        possessive_renderer = PossessiveRenderer()

        mandatory_bigram2contraction = {
            ('it', POSSESSIVE_MARK): 'its',
        }

        bigram2contraction = {
            ('will', 'not'): "won't",
        }

        def make_table(text):
            handle_row = None
            handle_column = None

            def handle_value(s):
                return {
                    '-': False,
                    'x': True,
                    'X': True,
                }

            return Table.init_from_text(
                text, handle_row, handle_column, handle_value)

        personal_table = make_table("""
                     I you he she it we they
            am       x -   -  -   -  -  -
            are      - x   -  -   -  x  x
            is       - -   x  x   x  -  -
            have_aux x x   -  -   -  x  x
            has_aux  - -   x  x   x  -  -
            had_aux  x x   x  x   x  x  x
            will     x x   x  x   x  x  x
            would    x x   x  x   x  x  x
        """)

        shortcut_table = make_table("""
                     this that here there what where when why how
            is       -    x    x    x     x    x     x    x   x
            have_aux -    -    -    -     x    x     -    -   x
            did      -    -    -    -     -    x     -    x   x
        """)

        verb_table = make_table("""
                am are is was were have_aux has_aux had_aux do does did
            not -  x   x  x   x    x        x       x       x  x    x
        """)

        modal_table = make_table("""
                     can could may might must should would will
            have_aux -   x     -   x     x    x      x     -
            not      x   x     -   -     x    x      x     X
        """)

        def make_s_second2contract(text):
            handle_row = None

            def handle_column(s):
                return {
                    'not_s': False,
                    's': True,
                }[s]

            def handle_value(s):
                return {
                    '-': False,
                    'x': True,
                }[s]

            table = Table.init_from_text(
                text, handle_row, handle_column, handle_value)
            return table.get_dict()

        s_second2contract = make_s_second2contract("""
                  not_s s
          has_aux x     -
          is      x     -
          %s      x     x
          will    x     x
          would   x     x
        """ % POSSESSIVE_MARK)

        def make_second2suffix(text):
            lines = text.strip().split('\n')
            assert len(lines) == 2
            sss = map(lambda line: line.split(), lines)
            assert len(sss[0]) == len(sss[1])
            return dict(zip(sss[0], sss[1]))

        second2suffix = make_second2suffix("""
            am are is have_aux has_aux had_aux did not will would %s
            'm 're 's 've      's      'd      'd  n't 'll  'd    's
        """ % POSSESSIVE_MARK)

        bigram2contract = {}
        for table in [personal_table, shortcut_table, verb_table, modal_table]:
            for row_second, column_first, is_contracted in table.each():
                assert row_second in second2suffix
                bigram = (column_first, row_second)
                bigram2contract[bigram] = is_contracted

        return BigramContractor(
            ends_with_s_detector, possessive_renderer,
            mandatory_bigram2contraction, bigram2contraction, bigram2contract,
            s_second2contract, second2suffix)

    def _key_from_token(self, token):
        if token.is_aux_verb():
            s = '%s_aux' % token.text()
        else:
            s = token.text()
        return s

    def maybe_contract(self, first, second, only_required_by_syntax=False):
        """
        (first, second) -> contracted word or None if no contraction.

        A third arg, only_required_by_syntax, means to only contract things like
        * it + 's = its
        * Tim + 's = Tim's
        """

        # Check if a contraction is even conceivable (for most words, no).
        second_key = self._key_from_token(second)
        if second_key not in self._seconds_set:
            return None

        # Syntax-mandated contractions (it + 's = its).
        first_key = self._key_from_token(first)
        bigram = (first_key, second_key)
        r = self._mandatory_bigram2contraction.get(bigram)
        if r:
            return r

        # 's is the only other mandatory type of contraction.
        if only_required_by_syntax and not second.is_possessive_mark():
            return None

        # Check the explicitly listed bigrams that have a custom contraction
        # (will + not = won't).
        r = self.bigram2contraction.get(bigram)
        if r:
            return r

        # Consult the explicitly listed bigrams.
        r = self._bigram2contract.get(bigram)
        if r is not None:
            if r:
                suffix = self._second2suffix[second_key]
                return first.text() + suffix
            else:
                return None

        # A handful of words form contractions most or all of the time (eg. "The
        # cat's friendly").
        ends_with_s = self._ends_with_s_detector.matches(first.text())
        s_pair = (ends_with_s, second_key)
        r = self._s_second2contract.get(s_pair)
        if r is not None:
            if r:
                suffix = self._second2suffix[second_key]
                return first.text() + suffix
            else:
                return None

        # Otherwise, don't contract the pair of words.
        return None
