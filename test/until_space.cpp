// Copyright 2017-2019 Elias Kosunen
//
// Licensed under the Apache License, Version 2.0 (the "License{");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file is a part of scnlib:
//     https://github.com/eliaskosunen/scnlib

#include "test.h"

TEST_CASE("read_into_until_space optimized")
{
    SUBCASE("regular")
    {
        auto stream = scn::make_stream("word another");
        auto locale = scn::basic_default_locale_ref<char>{};
        std::string buf;

        auto ret =
            scn::read_into_until_space(stream, locale, std::back_inserter(buf));

        CHECK(ret);
        CHECK(ret.value() == strlen("word"));
        CHECK(buf == "word");

        stream.read_char();  // skip space

        buf.clear();
        ret =
            scn::read_into_until_space(stream, locale, std::back_inserter(buf));

        CHECK(ret);
        CHECK(ret.value() == strlen("another"));
        CHECK(buf == "another");
    }
    SUBCASE("keep_final")
    {
        auto stream = scn::make_stream("word another");
        auto locale = scn::basic_default_locale_ref<char>{};
        std::string buf;

        auto ret = scn::read_into_until_space(stream, locale,
                                              std::back_inserter(buf), true);

        CHECK(ret);
        CHECK(ret.value() == strlen("word "));
        CHECK(buf == "word ");

        buf.clear();
        ret = scn::read_into_until_space(stream, locale,
                                         std::back_inserter(buf), true);

        CHECK(ret);
        CHECK(ret.value() == strlen("another"));
        CHECK(buf == "another");
    }
}

TEST_CASE("read sized")
{
    auto stream = scn::make_stream("abcde");

    SUBCASE("correct size span")
    {
        scn::detail::array<char, 6> buf{{0}};
        auto s = scn::make_span(buf).first(5);
        auto ret = scn::read(stream, s);
        CHECK(ret);
        CHECK(ret.value() == s.end());
        CHECK(std::string{buf.data()} == "abcde");
    }
    SUBCASE("undersized span")
    {
        scn::detail::array<char, 5> buf{{0}};
        auto s = scn::make_span(buf).first(4);
        auto ret = scn::read(stream, s);
        CHECK(ret);
        CHECK(ret.value() == s.end());
        CHECK(std::string{buf.data()} == "abcd");
    }
    SUBCASE("oversized span")
    {
        scn::detail::array<char, 7> buf{{0}};
        auto s = scn::make_span(buf).first(6);
        auto ret = scn::read(stream, s);
        CHECK(ret);
        CHECK(ret.value() == s.first(5).end());
        CHECK(std::string{buf.data()} == "abcde");
    }
}
TEST_CASE("read non-sized")
{
    auto stream = make_nonsized_stream(scn::make_stream("abcde"));

    SUBCASE("correct size span")
    {
        scn::detail::array<char, 6> buf{{0}};
        auto s = scn::make_span(buf).first(5);
        auto ret = scn::read(stream, s);
        CHECK(ret);
        CHECK(ret.value() == s.end());
        CHECK(std::string{buf.data()} == "abcde");
    }
    SUBCASE("undersized span")
    {
        scn::detail::array<char, 5> buf{{0}};
        auto s = scn::make_span(buf).first(4);
        auto ret = scn::read(stream, s);
        CHECK(ret);
        CHECK(ret.value() == s.end());
        CHECK(std::string{buf.data()} == "abcd");
    }
    SUBCASE("oversized span")
    {
        scn::detail::array<char, 7> buf{{0}};
        auto s = scn::make_span(buf).first(6);
        auto ret = scn::read(stream, s);
        CHECK(ret);
        CHECK(ret.value() == s.first(5).end());
        CHECK(std::string{buf.data()} == "abcde");
    }
}

TEST_CASE("putback_range")
{
    SUBCASE("sized")
    {
        auto stream = scn::make_stream("foo");
        scn::detail::array<char, 4> buf{{0}};
        auto s = scn::make_span(buf).first(3);
        {
            auto ret = scn::read(stream, s);
            CHECK(ret);
            CHECK(ret.value() == s.end());
            CHECK(std::strcmp(buf.data(), "foo") == 0);
        }
        auto err = scn::putback_range(stream, s.begin(), s.end());
        CHECK(err);
        {
            auto ret = scn::read(stream, s);
            CHECK(ret);
            CHECK(ret.value() == s.end());
            CHECK(std::strcmp(buf.data(), "foo") == 0);
        }
    }
    SUBCASE("non-sized")
    {
        auto stream = make_nonsized_stream(scn::make_stream("foo"));
        scn::detail::array<char, 4> buf{{0}};
        auto s = scn::make_span(buf).first(3);
        {
            auto ret = scn::read(stream, s);
            CHECK(ret);
            CHECK(ret.value() == s.end());
            CHECK(std::strcmp(buf.data(), "foo") == 0);
        }
        auto err = scn::putback_range(stream, s.begin(), s.end());
        CHECK(err);
        {
            auto ret = scn::read(stream, s);
            CHECK(ret);
            CHECK(ret.value() == s.end());
            CHECK(std::strcmp(buf.data(), "foo") == 0);
        }
    }
}
