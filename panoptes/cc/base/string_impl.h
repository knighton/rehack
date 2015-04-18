template <typename int_t>
bool String::ToInt(const string& from, size_t* x, int_t* to) {
    if (from.size() <= *x) {
        return false;
    }

    *to = 0;
    bool positive = true;

    if (from[*x] == '-') {
        positive = false;
        ++(*x);
    } else if (from[*x] == '+') {
        ++(*x);
    }

    if (*x == from.size()) {
        return false;
    }

    bool got_a_digit = false;
    for (; *x < from.size(); ++(*x)) {
        if (from[*x] < '0' || '9' < from[*x]) {
            break;
        } else {
            got_a_digit = true;
            *to *= 10;
            *to += static_cast<int_t>(from[*x] - '0');
        }
    }

    if (!positive) {
        *to *= static_cast<int_t>(-1);
    }

    return got_a_digit;
}

template <typename int_t>
bool String::ToInt(const string& from, int_t* to) {
    size_t x = 0;
    return String::ToInt(from, &x, to);
}

template <typename float_t>
bool String::ToFloat(const string& from, size_t* x, float_t* to) {
    if (from.size() <= *x) {
        return false;
    }

    *to = 0;
    bool positive = true;

    if (from[*x] == '-') {
        positive = false;
        ++(*x);
    } else if (from[*x] == '+') {
        ++(*x);
    }

    if (*x == from.size()) {
        return false;
    }

    bool got_a_digit = false;
    for (; *x < from.size(); ++(*x)) {
        char c = from[*x];
        if (c == '.') {
            break;
        } else if (c < '0' || '9' < c) {
            break;
        } else {
            got_a_digit = true;
            *to *= 10;
            *to += c - '0';
        }
    }

    if (from[*x] == '.') {
        ++(*x);
        float_t power = 1.0;

        for (; *x < from.size(); ++(*x)) {
            char c = from[*x];
            if (c < '0' || '9' < c) {
                break;
            } else {
            got_a_digit = true;
                power /= 10;
                *to += (c - '0') * power;
            }
        }
    }

    if (!positive) {
        *to *= -1;
    }

    return got_a_digit;
}
