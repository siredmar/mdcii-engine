#include <gmock/gmock.h>

#include <mdcii/cache/cacheprotobuf.hpp>

#include "mdcii/cod/text_cod.hpp"
#include "textcod.pb.h"

class TextCodTest : public testing::Test
{
public:
    // clang-format off
    std::string testCodString = 
"-------------------------------------------------- \n"
"[ERROR] \n"
"Error message 0! \n"
"Error message 1! \n"
"Error message 2! \n"
"Error message 3! \n"
"Error message 4! \n"
"[END] \n"
"-------------------------------------------------- \n"
"[PATH] \n"
"Some Path \n"
"[END] \n"
"-------------------------------------------------- \n"
"[ENDLESSKIND] \n"
"Easy \n"
"Average \n"
"Somewhat okay \n"
"Hard \n"
"\n"
"[END] \n"
"-------------------------------------------------- \n"
"[KAMPAGNE] \n"
"KMP0_MISSION0 \n"
"KMP0_MISSION1 \n"
"KMP0_MISSION2 \n"
"KMP0_MISSION3 \n"
" \n"
"KMP1_MISSION0 \n"
"KMP1_MISSION1 \n"
"KMP1_MISSION2 \n"
" \n"
" \n"
"KMP2_MISSION0 \n"
"KMP2_MISSION1 \n"
"KMP2_MISSION2 \n"
" \n"
" \n"
"KMP3_MISSION0 \n"
"KMP3_MISSION1 \n"
"KMP3_MISSION2 \n"
" \n"
" \n"
"KMP4_MISSION0 \n"
"KMP4_MISSION1 \n"
"KMP4_MISSION2 \n"
" \n"
" \n"
"KMP5_MISSION0 \n"
"KMP5_MISSION1 \n"
"KMP5_MISSION2 \n"
"KMP5_MISSION3 \n"
" \n"
"[END] \n"
"-------------------------------------------------- \n"
"[FIGKIND] \n"
"Unused \n"
"FIG0 \n"
"FIG1 \n"
"FIG2 \n"
"FIG3 \n"
"[END]";
    // clang-format on
};

TEST_F(TextCodTest, ReadSingleBlockRawProtobuf)
{
    auto textCod = TextCod(testCodString);
    auto s = textCod.GetSection("KAMPAGNE");
    ASSERT_EQ(s->value_size(), 30);
    ASSERT_EQ(s->value(0), "KMP0_MISSION0");
    ASSERT_EQ(s->value(10), "KMP2_MISSION0");
    ASSERT_EQ(s->value(28), "KMP5_MISSION3");
    ASSERT_EQ(s->value(29), "");
    auto endless = textCod.GetSection("ENDLESSKIND");
    ASSERT_EQ(endless->value_size(), 4);
    ASSERT_EQ(endless->value(2), "Somewhat okay");
}

TEST_F(TextCodTest, ReadSingleBlockGetter)
{
    auto textCod = TextCod(testCodString);
    ASSERT_EQ(textCod.GetSectionSize("KAMPAGNE"), 30);
    ASSERT_EQ(textCod.GetValue("KAMPAGNE", 0), "KMP0_MISSION0");
    ASSERT_EQ(textCod.GetValue("KAMPAGNE", 10), "KMP2_MISSION0");
    ASSERT_EQ(textCod.GetValue("KAMPAGNE", 28), "KMP5_MISSION3");
    ASSERT_EQ(textCod.GetValue("KAMPAGNE", 29), "");
    ASSERT_EQ(textCod.GetSectionSize("ENDLESSKIND"), 4);
    ASSERT_EQ(textCod.GetValue("ENDLESSKIND", 2), "Somewhat okay");
}

TEST_F(TextCodTest, InvalidSection)
{
    auto textCod = TextCod(testCodString);
    ASSERT_ANY_THROW(textCod.GetSectionSize("INVALIDSECTION"));
    ASSERT_ANY_THROW(textCod.GetValue("INVALIDSECTION", 10));
}

TEST_F(TextCodTest, ValidSectionInvalidValue)
{
    auto textCod = TextCod(testCodString);
    ASSERT_EQ(textCod.GetSectionSize("FIGKIND"), 5);
    ASSERT_ANY_THROW(textCod.GetValue("FIGKIND", 6));
}