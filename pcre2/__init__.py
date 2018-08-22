import _pcre2

I = IGNORECASE = _pcre2.CASELESS
M = MULTILINE = _pcre2.MULTILINE
S = DOTALL = _pcre2.DOTALL
X = VERBOSE = _pcre2.EXTENDED


class Pattern:
    def __init__(self, pattern, flags=0):
        self._pattern = _pcre2.Pattern(pattern, flags)

    def finditer(self, text):
        context = self._pattern.match(text)
        while context:
            yield Match(context)
            context = self._pattern.match(context)

    @property
    def flags(self):
        return self._pattern.flags

    @property
    def groups(self):
        return self._pattern.groups

    @property
    def groupindex(self):
        return self._pattern.groupindex

    @property
    def pattern(self):
        return self._pattern.pattern


class Match:
    def __init__(self, context):
        match = context.to_match()
        self._groups = match.groups
        self._groupdict = match.groupmap

        self._pos = context.pos
        self._endpos = context.endpos

        self._re = context.pattern
        self._string = context.text

    def group(self, index):
        group = self._groups[index]
        return group.text if group else None

    def start(self, index=0):
        group = self._groups[index]
        return group.start if group else -1

    def end(self, index=0):
        group = self._groups[index]
        return group.end if group else -1

    def span(self, index=0):
        return (self.start(index), self.end(index))

    @property
    def pos(self):
        return self._pos

    @property
    def endpos(self):
        return self._endpos

    @property
    def re(self):
        return self._re

    @property
    def string(self):
        return self._string

    def __repr__(self):
        return '<Match span=({}, {}), match={}>'.format(*self.span(), repr(self.group(0)))