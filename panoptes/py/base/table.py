class Table(object):
    """
    An embedded table in source code -> a dict and two lists.
    """

    def __init__(self, row_keys, column_keys, row_column2value):
        self._row_keys = row_keys
        self._column_keys = column_keys
        self._row_column2value = row_column2value

    @staticmethod
    def init_from_text(text, handle_row_key, handle_column_key, handle_value):
        """
        (table text, func or None, func or None, func or None) -> Table.
        """
        lines = text.strip().split('\n')
        sss = []
        for line in lines:
            ss = line.split()
            sss.append(ss)

        if len(sss):
            n = len(sss[0])
            for ss in sss[1:]:
                assert len(ss) == n + 1

        row_keys = []
        for i in xrange(1, len(sss)):
            s = sss[i][0]
            if handle_row_key:
                s = handle_row_key(s)
            row_keys.append(s)

        column_keys = []
        for i in xrange(1, len(sss[0])):
            s = sss[0][i]
            if handle_column_key:
                s = handle_column_key(s)
            column_keys.append(s)

        row_column2value = {}
        for i in xrange(len(row_keys)):
            for j in xrange(len(column_keys)):
                s = sss[i + 1][j + 1]
                if handle_value:
                    value = handle_value(s)
                else:
                    value = s
                row_key = row_keys[i]
                column_key = column_keys[j]
                row_column2value[(row_key, column_key)] = value

        return Table(row_keys, column_keys, row_column2value)

    def rows(self):
        return self._row_keys

    def columns(self):
        return self._column_keys

    def get(self, row_key, column_key):
        return self._row_column2value[(row_key, column_key)]

    def get_or(self, row_key, column_key, missing):
        return self._row_column2value.get((row_key, column_key), missing)

    def get_dict(self):
        return self._row_column2value

    def each(self):
        for i in xrange(len(self._row_keys)):
            for j in xrange(len(self._column_keys)):
                row = self._row_keys[i]
                column = self._column_keys[j]
                value = self._row_column2value[(row, column)]
                yield row, column, value
