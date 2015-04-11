import copy
import json


RELS = 'agent target because'.split()
UNIVERSAL_RELS = 'because'.split()


UNK = '<WHO/WHAT/ETC>'


class Person(object):
    def __init__(self, name, gender, age):
        self.name = name
        self.gender = gender
        self.age = age


class VerbSpec(object):
    def __init__(self, lemma, rels):
        self.lemma = lemma
        self.rels = rels


class DeepClause(object):
    def __init__(self, verb, rel2name):
        self.verb = verb
        self.rel2name = rel2name

    def to_d(self):
        return {
            'verb': self.verb,
            'rel2name': self.rel2name,
        }


class BrainClause(object):
    def __init__(self, verb, rel2x):
        self.verb = verb
        self.rel2x = rel2x


class Brain(object):
    def __init__(self):
        self.items = []
        self.name2x = {}
        self.verb2spec = {}

        self.add_person(Person('Tim', 'm', 28))
        self.add_person(Person('Tom', 'm', 26))

        self.verb2spec['see'] = VerbSpec('see', ['agent', 'target'])

    def add_person(self, person):
        assert person.name not in self.name2x
        self.name2x[person.name] = len(self.items)
        self.items.append(person)

    def convert_to_inside(self, dc):
        rel2x = {}
        for rel, name in dc.rel2name.iteritems():
            x = self.name2x[name]
            rel2x[rel] = x
        return BrainClause(dc.verb, rel2x)

    def convert_to_outside(self, bc):
        rel2name = {}
        for rel, x in bc.rel2x.iteritems():
            if x is None:
                name = UNK
            else:
                name = self.items[x].name
            rel2name[rel] = name
        return DeepClause(bc.verb, rel2name)

    def think(self, bc):
        for rel in UNIVERSAL_RELS:
            if rel not in bc.rel2x:
                new_bc = copy.deepcopy(bc)
                new_bc.rel2x[rel] = None
                yield new_bc

    def receive(self, dc):
        brain_clause = self.convert_to_inside(dc)
        for bc in self.think(brain_clause):
            yield self.convert_to_outside(bc)


def main():
    b = Brain()
    c = DeepClause('see', {
        'agent': 'Tim',
        'target': 'Tom',
    })
    for clause in b.receive(c):
        print json.dumps(clause.to_d(), indent=4)


if __name__ == '__main__':
    main()
