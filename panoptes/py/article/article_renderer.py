import re

from py.article import config


class ArticleRenderer(object):
    """
    Chooses a or an.
    """

    def __init__(self, patterns_aorans):
        # Exhaustive list of regexes paired with an (a or an) choice.
        self.regexes_aorans = []
        for pattern, a_or_an in patterns_aorans:
            r = re.compile(pattern)
            assert a_or_an in ('a', 'an')
            self.regexes_aorans.append((r, a_or_an))
        assert self.regexes_aorans

    @staticmethod
    def init_default():
        return ArticleRenderer(config.A_OR_AN)

    def a_or_an(self, next_word):
        """
        next word (string) -> whether this word is a or an.
        """
        for rgx, a_or_an in self.regexes_aorans:
            if rgx.match(next_word):
                return a_or_an
        assert False
