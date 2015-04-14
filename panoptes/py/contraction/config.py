from py.base.table import Table
from py.base.token import POSSESSIVE_MARK


ENDS_WITH_S_SOUND = """
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
""".split()


MANDATORY_BIGRAM2CONTRACTION = {
    ('it', POSSESSIVE_MARK): 'its',
}


BIGRAM2CONTRACTION = {
    ('will', 'not'): "won't",
}


def _make_table(text):
    handle_row = None
    handle_column = None

    def handle_value(s):
        return {
            '-': False,
            'x': True,
            'X': True,
        }

    return Table.init_from_text(text, handle_row, handle_column, handle_value)


PERSONAL_BIGRAMS = _make_table("""
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


CORRELATIVE_BIGRAMS = _make_table("""
             this that here there what where when why how
    is       -    x    x    x     x    x     x    x   x
    have_aux -    -    -    -     x    x     -    -   x
    did      -    -    -    -     -    x     -    x   x
""")


VERB_BIGRAMS = _make_table("""
        am are is was were have_aux has_aux had_aux do does did
    not -  x   x  x   x    x        x       x       x  x    x
""")


MODAL_BIGRAMS = _make_table("""
             can could may might must should would will
    have_aux -   x     -   x     x    x      x     -
    not      x   x     -   -     x    x      x     X
""")


def _make_s_second2contract(text):
    def handle_row(s):
        return {
            'not_s': False,
            's': True,
        }[s]

    handle_column = None

    def handle_value(s):
        return {
            '-': False,
            'x': True,
        }[s]

    table = Table.init_from_text(
        text, handle_row, handle_column, handle_value)
    return table.get_dict()


S_SECOND2CONTRACT = _make_s_second2contract("""
          has_aux is %s will would
    not_s x       x  x  x    x
    s     -       -  x  x    x
""" % POSSESSIVE_MARK)


def _make_second2suffix(text):
    lines = text.strip().split('\n')
    assert len(lines) == 2
    sss = map(lambda line: line.split(), lines)
    assert len(sss[0]) == len(sss[1])
    return dict(zip(sss[0], sss[1]))


SECOND2SUFFIX = _make_second2suffix("""
    am are is have_aux has_aux had_aux did not will would %s
    'm 're 's 've      's      'd      'd  n't 'll  'd    %s
""" % (POSSESSIVE_MARK, POSSESSIVE_MARK))
