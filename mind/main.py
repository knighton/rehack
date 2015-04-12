import json

from mind import Mind
from english import DeepClause, UNK


def main():
    m = Mind()
    clauses = [
        DeepClause('see', {
            'agent': 'Tim',
            'target': 'Tom',
        }),
        DeepClause('see', {
            'agent': UNK,
            'target': 'Tom',
        }),
    ]

    for c in clauses:
        print '-' * 80
        print c.to_d()
        for clause in m.receive(c):
            print '*', json.dumps(clause.to_d(), indent=4)


if __name__ == '__main__':
    main()
