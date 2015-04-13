import copy
import json

from english import DeepClause, UNK, UNIVERSAL_RELS



class Person(object):
    def __init__(self, name, gender, age):
        self.name = name
        self.gender = gender
        self.age = age


class VerbSpec(object):
    def __init__(self, lemma, rels):
        self.lemma = lemma
        self.rels = rels


class MindClause(object):
    def __init__(self, verb, rel2x):
        self.verb = verb
        self.rel2x = rel2x

    def is_q(self):
        for rel, x in self.rel2x.iteritems():
            if x is None:
                return True
        return False

    def get_pattern(self):
        ss = []
        ss.append(self.verb)
        for rel, x in self.rel2x.iteritems():
            if x is not None:
                ss.append(rel)
        return ss

    def accepts_pattern(self, pattern):
        verb = pattern[0]
        if verb != self.verb:
            return False
        for rel in pattern[1:]:
            if rel not in self.rel2x:
                return False
        return True

    def get_wildcard_rel(self):
        rels = []
        for rel, x in self.rel2x.iteritems():
            if x is None:
                rels.append(rel)
        assert len(rels) == 1
        rel = rels[0]
        return rel


class Mind(object):
    def __init__(self):
        self.items = []
        self.name2x = {}
        self.verb2spec = {}
        self.facts = []

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
            x = self.name2x.get(name, None)
            rel2x[rel] = x
        return MindClause(dc.verb, rel2x)

    def convert_to_outside(self, bc):
        rel2name = {}
        for rel, x in bc.rel2x.iteritems():
            if x is None:
                name = UNK
            else:
                name = self.items[x].name
            rel2name[rel] = name
        return DeepClause(bc.verb, rel2name)

    def get_relevant_facts(self, pattern):
        facts = []
        for fact in self.facts:
            if fact.accepts_pattern(pattern):
                facts.append(fact)
        return facts

    def think_about_question(self, bc):
        pattern = bc.get_pattern()
        facts = self.get_relevant_facts(pattern)
        assert len(facts) == 1
        fact = facts[0]
        wildcard_rel = bc.get_wildcard_rel()

        notable_x = fact.rel2x[wildcard_rel]
        print 'Notable argument:', self.items[notable_x].name

        verb = pattern[0]
        rel2x = {}
        for rel in pattern[1:]:
            rel2x[rel] = bc.rel2x[rel]
        rel2x[wildcard_rel] = fact.rel2x[wildcard_rel]
        bc = MindClause(verb, rel2x)
        yield bc

    def think_about_statement(self, bc):
        self.facts.append(bc)
        for rel in UNIVERSAL_RELS:
            if rel not in bc.rel2x:
                new_bc = copy.deepcopy(bc)
                new_bc.rel2x[rel] = None
                yield new_bc

    def think_about(self, bc):
        if bc.is_q():
            f = self.think_about_question
        else:
            f = self.think_about_statement
        for bc in f(bc):
            yield bc

    def receive(self, dc):
        mind_clause = self.convert_to_inside(dc)
        for bc in self.think_about(mind_clause):
            yield self.convert_to_outside(bc)
