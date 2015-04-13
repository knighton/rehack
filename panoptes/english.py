RELS = 'agent target because'.split()
UNIVERSAL_RELS = 'because'.split()


UNK = 'WHO'


class DeepClause(object):
    def __init__(self, verb, rel2name):
        self.verb = verb
        self.rel2name = rel2name

    def to_d(self):
        return {
            'verb': self.verb,
            'rel2name': self.rel2name,
        }
