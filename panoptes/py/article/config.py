# a/an-picking from the Linguistics ruby gem by Michael Granger:
# http://www.deveiate.org/projects/Linguistics/wiki/English
A_OR_AN = [
    ('euler|hour(?!i)|heir|honest|hono', 'an'),  # Exceptions (an honor).

    # Abbreviations:
    # Strings of capitals starting with a vowel-sound consonant followed by
    # another consonant, and which are not likely to be real words.
    ('(?!FJO|[HLMNS]Y.|RY[EO]|SQU|(F[LR]?|[HL]|MN?|N|RH?|S[CHKLMNPTVW]?|'
     'X(YL)?)[AEIOU])[FHLMNRSX][A-Z]', 'an'),

    ('^[aefhilmnorsx][.-]', 'an'),
    ('^[a-z][.-]', 'a'),

    ('^[^aeiouy]', 'a'),
    ('^e[uw]', 'a'),                 # eu like "you" (a european).
    ('^onc?e', 'a'),                 # o like "wa" (a one-liner).
    ('uni([^nmd]|mo)', 'a'),         # u like "you" (a university).
    ('^u[bcfhjkqrst][aeiou]', 'a'),  # u like "you" (a uterus).
    ('^[aeiou]', 'an'),              # Vowels (an owl).
    ('y(b[lor]|cl[ea]|fere|gg|p[ios]|rou|tt)', 'an'),  # y like i (an yclept)
    ('', 'a')                        # Guess "a".
]
