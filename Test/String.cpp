#include <Utils.h>

#include "Catch.hpp"

using namespace AGZ;

TEST_CASE("String")
{
    SECTION("UTF-8")
    {
        REQUIRE(UTF8<char>::Check("0123456789", 10));

        uint8_t data[4] = { 0, 1, 2, 255 };
        REQUIRE(!UTF8<uint8_t>::Check(data, 4));
    }

    SECTION("Constructor")
    {
        REQUIRE(Str(u8"minecraft", 9).Length() == 9);
        REQUIRE(Str(u8"ABCDEFGHIJKLMNOPQRSTUWVZYAABCDEFGHIJKLMNOPQRSTUWVZYA"
                      "ABCDEFGHIJKLMNOPQRSTUWVZYAABCDEFGHIJKLMNOPQRSTUWVZYA", 104).Length() == 104);
        REQUIRE(Str(u8"ABCDEFGHIJKLMNOPQRSTUWVZYAABCDEFGHIJKLMNOPQRSTUWVZYA"
                      "ABCDEFGHIJKLMNOPQRSTUWVZYAABCDEFGHIJKLMNOPQRSTUWVZYA", 104)[4] == u8'E');
        REQUIRE(Str(u8"今", 3).Length() == 3);
    }
}
