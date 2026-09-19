// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <tuple>
#include <utility>

#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <pathstatus.h>

static std::map<std::string, std::tuple<constr_t, bool_t>> PathExistsD
{
    {"PathExists1", std::make_tuple("/home/kerman/projects/GoogleTestC/testfolder/donotdeletetestfile.txt", 1)},
    {"PathExists2", std::make_tuple("/home/kerman/projects/GoogleTestC/testfolder", 1)},
    {"PathDoesNotExist1", std::make_tuple("/home/kerman/projects/GoogleTestC/testfolder/invalidfile.txt", 0)},
    {"PathDoesNotExist2", std::make_tuple("/home/kerman/projects/GoogleTestC/testfolder/invalidfolder", 0)}
};

class PathExistsF : public testing::TestWithParam<std::pair<const std::string, std::tuple<constr_t, bool_t>>>
{
};

INSTANTIATE_TEST_SUITE_P(pathstatus, PathExistsF, testing::ValuesIn(PathExistsD),
[] (const testing::TestParamInfo<PathExistsF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(PathExistsF, IsOk)
{
    // Arrange
    std::tuple<constr_t, bool_t> tuple = GetParam().second;
    constr_t fullPathName = std::get<0>(tuple); 
    const bool_t expPathExists = std::get<1>(tuple);

    // Act
    struct stat pathStatus;
    bool_t actPathExists = PathExists(fullPathName, &pathStatus);

    // Assert
    EXPECT_EQ((int) expPathExists, (int) actPathExists);
}