#include <gtest/gtest.h>
#include <OpenXLSX.hpp>
#include <filesystem>
#include <fstream>
#include "execlParser.h"
#include <bite_scaffold/log.h>

namespace chat2excel {
namespace test {

class ExcelParserTest : public ::testing::Test {
protected:
    static std::string testDir;
    static std::string singleSheetFile;
    static std::string multiSheetFile;
    static std::string emptySheetFile;
    static std::string variousTypesFile;

    static void SetUpTestSuite() {
        testDir = std::filesystem::current_path().string() + "/test_data";
        std::filesystem::create_directories(testDir);
        
        singleSheetFile = testDir + "/single_sheet.xlsx";
        multiSheetFile = testDir + "/multi_sheet.xlsx";
        emptySheetFile = testDir + "/empty_sheet.xlsx";
        variousTypesFile = testDir + "/various_types.xlsx";
        
        createSingleSheetExcel();
        createMultiSheetExcel();
        createEmptySheetExcel();
        createVariousTypesExcel();
    }

    static void TearDownTestSuite() {
        std::filesystem::remove_all(testDir);
    }

    static void createSingleSheetExcel() {
        OpenXLSX::XLDocument doc;
        doc.create(singleSheetFile);
        
        auto wks = doc.workbook().worksheet(1);
        wks.setName("Sheet1");
        
        wks.cell("A1").value() = "Name";
        wks.cell("B1").value() = "Age";
        wks.cell("C1").value() = "City";
        
        wks.cell("A2").value() = "Alice";
        wks.cell("B2").value() = 25;
        wks.cell("C2").value() = "Beijing";
        
        wks.cell("A3").value() = "Bob";
        wks.cell("B3").value() = 30;
        wks.cell("C3").value() = "Shanghai";
        
        wks.cell("A4").value() = "Charlie";
        wks.cell("B4").value() = 35;
        wks.cell("C4").value() = "Guangzhou";
        
        doc.save();
    }

    static void createMultiSheetExcel() {
        OpenXLSX::XLDocument doc;
        doc.create(multiSheetFile);
        
        auto wks1 = doc.workbook().worksheet(1);
        wks1.setName("Employees");
        
        wks1.cell("A1").value() = "ID";
        wks1.cell("B1").value() = "Name";
        wks1.cell("C1").value() = "Department";
        
        wks1.cell("A2").value() = 1001;
        wks1.cell("B2").value() = "John";
        wks1.cell("C2").value() = "Engineering";
        
        wks1.cell("A3").value() = 1002;
        wks1.cell("B3").value() = "Jane";
        wks1.cell("C3").value() = "Marketing";
        
        doc.workbook().addWorksheet("Products");
        auto wks2 = doc.workbook().worksheet("Products");
        
        wks2.cell("A1").value() = "ProductID";
        wks2.cell("B1").value() = "ProductName";
        wks2.cell("C1").value() = "Price";
        
        wks2.cell("A2").value() = "P001";
        wks2.cell("B2").value() = "Laptop";
        wks2.cell("C2").value() = 5999.99;
        
        wks2.cell("A3").value() = "P002";
        wks2.cell("B3").value() = "Mouse";
        wks2.cell("C3").value() = 99.50;
        
        doc.workbook().addWorksheet("Orders");
        auto wks3 = doc.workbook().worksheet("Orders");
        
        wks3.cell("A1").value() = "OrderID";
        wks3.cell("B1").value() = "Customer";
        wks3.cell("C1").value() = "Amount";
        
        wks3.cell("A2").value() = "ORD001";
        wks3.cell("B2").value() = "CompanyA";
        wks3.cell("C2").value() = 15000;
        
        doc.save();
    }

    static void createEmptySheetExcel() {
        OpenXLSX::XLDocument doc;
        doc.create(emptySheetFile);
        
        auto wks = doc.workbook().worksheet(1);
        wks.setName("EmptySheet");
        
        doc.save();
    }

    static void createVariousTypesExcel() {
        OpenXLSX::XLDocument doc;
        doc.create(variousTypesFile);
        
        auto wks = doc.workbook().worksheet(1);
        wks.setName("DataTypes");
        
        wks.cell("A1").value() = "Integer";
        wks.cell("B1").value() = "Float";
        wks.cell("C1").value() = "String";
        wks.cell("D1").value() = "Boolean";
        wks.cell("E1").value() = "Empty";
        
        wks.cell("A2").value() = 42;
        wks.cell("B2").value() = 3.14159;
        wks.cell("C2").value() = "Hello World";
        wks.cell("D2").value() = true;
        wks.cell("E2").value() = OpenXLSX::XLCellValue();
        
        wks.cell("A3").value() = -100;
        wks.cell("B3").value() = 2.5;
        wks.cell("C3").value() = "Test String";
        wks.cell("D3").value() = false;
        wks.cell("E3").value() = OpenXLSX::XLCellValue();
        
        wks.cell("A4").value() = 0;
        wks.cell("B4").value() = 0.0;
        wks.cell("C4").value() = "";
        wks.cell("D4").value() = true;
        
        wks.cell("A5").value() = 9999999999;
        wks.cell("B5").value() = 123.456789;
        wks.cell("C5").value() = "Special Characters: !@#$%^&*()";
        wks.cell("D5").value() = false;
        
        doc.save();
    }
};

std::string ExcelParserTest::testDir;
std::string ExcelParserTest::singleSheetFile;
std::string ExcelParserTest::multiSheetFile;
std::string ExcelParserTest::emptySheetFile;
std::string ExcelParserTest::variousTypesFile;

TEST_F(ExcelParserTest, GetWorksheetNames_SingleSheet) {
    auto names = ExcelParser::getWorksheetNames(singleSheetFile);
    
    ASSERT_EQ(names.size(), 1);
    EXPECT_EQ(names[0], "Sheet1");
}

TEST_F(ExcelParserTest, GetWorksheetNames_MultiSheet) {
    auto names = ExcelParser::getWorksheetNames(multiSheetFile);
    
    ASSERT_EQ(names.size(), 3);
    
    bool hasEmployees = false;
    bool hasProducts = false;
    bool hasOrders = false;
    
    for (const auto& name : names) {
        if (name == "Employees") hasEmployees = true;
        if (name == "Products") hasProducts = true;
        if (name == "Orders") hasOrders = true;
    }
    
    EXPECT_TRUE(hasEmployees);
    EXPECT_TRUE(hasProducts);
    EXPECT_TRUE(hasOrders);
}

TEST_F(ExcelParserTest, GetWorksheetNames_EmptySheet) {
    auto names = ExcelParser::getWorksheetNames(emptySheetFile);
    
    ASSERT_EQ(names.size(), 1);
    EXPECT_EQ(names[0], "EmptySheet");
}

TEST_F(ExcelParserTest, GetWorksheetNames_NonExistentFile) {
    auto names = ExcelParser::getWorksheetNames("/non/existent/file.xlsx");
    
    EXPECT_TRUE(names.empty());
}

TEST_F(ExcelParserTest, GetWorksheetData_SingleSheet) {
    auto data = ExcelParser::getWorksheetData(singleSheetFile, "Sheet1");
    
    EXPECT_FALSE(data.empty());
    
    EXPECT_TRUE(data.find("Name") != std::string::npos);
    EXPECT_TRUE(data.find("Age") != std::string::npos);
    EXPECT_TRUE(data.find("City") != std::string::npos);
    EXPECT_TRUE(data.find("Alice") != std::string::npos);
    EXPECT_TRUE(data.find("Bob") != std::string::npos);
    EXPECT_TRUE(data.find("Charlie") != std::string::npos);
    EXPECT_TRUE(data.find("Beijing") != std::string::npos);
    EXPECT_TRUE(data.find("Shanghai") != std::string::npos);
    EXPECT_TRUE(data.find("Guangzhou") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_MultiSheet_FirstSheet) {
    auto data = ExcelParser::getWorksheetData(multiSheetFile, "Employees");
    
    EXPECT_FALSE(data.empty());
    
    EXPECT_TRUE(data.find("ID") != std::string::npos);
    EXPECT_TRUE(data.find("Name") != std::string::npos);
    EXPECT_TRUE(data.find("Department") != std::string::npos);
    EXPECT_TRUE(data.find("John") != std::string::npos);
    EXPECT_TRUE(data.find("Jane") != std::string::npos);
    EXPECT_TRUE(data.find("Engineering") != std::string::npos);
    EXPECT_TRUE(data.find("Marketing") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_MultiSheet_SecondSheet) {
    auto data = ExcelParser::getWorksheetData(multiSheetFile, "Products");
    
    EXPECT_FALSE(data.empty());
    
    EXPECT_TRUE(data.find("ProductID") != std::string::npos);
    EXPECT_TRUE(data.find("ProductName") != std::string::npos);
    EXPECT_TRUE(data.find("Price") != std::string::npos);
    EXPECT_TRUE(data.find("Laptop") != std::string::npos);
    EXPECT_TRUE(data.find("Mouse") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_MultiSheet_ThirdSheet) {
    auto data = ExcelParser::getWorksheetData(multiSheetFile, "Orders");
    
    EXPECT_FALSE(data.empty());
    
    EXPECT_TRUE(data.find("OrderID") != std::string::npos);
    EXPECT_TRUE(data.find("Customer") != std::string::npos);
    EXPECT_TRUE(data.find("Amount") != std::string::npos);
    EXPECT_TRUE(data.find("ORD001") != std::string::npos);
    EXPECT_TRUE(data.find("CompanyA") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_NonExistentWorksheet) {
    auto data = ExcelParser::getWorksheetData(multiSheetFile, "NonExistentSheet");
    
    EXPECT_TRUE(data.empty());
}

TEST_F(ExcelParserTest, GetWorksheetData_NonExistentFile) {
    auto data = ExcelParser::getWorksheetData("/non/existent/file.xlsx", "Sheet1");
    
    EXPECT_TRUE(data.empty());
}

TEST_F(ExcelParserTest, GetWorksheetData_EmptySheet) {
    auto data = ExcelParser::getWorksheetData(emptySheetFile, "EmptySheet");
    
    EXPECT_TRUE(data.empty());
}

TEST_F(ExcelParserTest, GetWorksheetData_VariousTypes_Integer) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    EXPECT_FALSE(data.empty());
    
    EXPECT_TRUE(data.find("42") != std::string::npos);
    EXPECT_TRUE(data.find("-100") != std::string::npos);
    EXPECT_TRUE(data.find("9999999999") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_VariousTypes_Float) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    EXPECT_FALSE(data.empty());
    
    EXPECT_TRUE(data.find("3.14159") != std::string::npos);
    EXPECT_TRUE(data.find("2.5") != std::string::npos);
    EXPECT_TRUE(data.find("123.456789") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_VariousTypes_String) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    EXPECT_FALSE(data.empty());
    
    EXPECT_TRUE(data.find("Hello World") != std::string::npos);
    EXPECT_TRUE(data.find("Test String") != std::string::npos);
    EXPECT_TRUE(data.find("Special Characters: !@#$%^&*()") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_VariousTypes_Boolean) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    EXPECT_FALSE(data.empty());
    
    std::istringstream iss(data);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    
    bool foundTrue = false;
    bool foundFalse = false;
    for (const auto& l : lines) {
        if (l.find(",1,") != std::string::npos || 
            l.find(",1") == l.length() - 2 ||
            l.find("1,") == 0) {
            foundTrue = true;
        }
        if (l.find(",0,") != std::string::npos || 
            l.find(",0") == l.length() - 2 ||
            l.find("0,") == 0) {
            foundFalse = true;
        }
    }
    
    EXPECT_TRUE(foundTrue || foundFalse);
}

TEST_F(ExcelParserTest, GetWorksheetData_CSVFormat) {
    auto data = ExcelParser::getWorksheetData(singleSheetFile, "Sheet1");
    
    EXPECT_FALSE(data.empty());
    
    std::istringstream iss(data);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    
    ASSERT_GE(lines.size(), 4);
    
    EXPECT_EQ(lines[0], "Name,Age,City");
    EXPECT_EQ(lines[1], "Alice,25,Beijing");
    EXPECT_EQ(lines[2], "Bob,30,Shanghai");
    EXPECT_EQ(lines[3], "Charlie,35,Guangzhou");
}

TEST_F(ExcelParserTest, GetWorksheetData_MultiSheet_AllSheetsReadable) {
    auto names = ExcelParser::getWorksheetNames(multiSheetFile);
    
    for (const auto& name : names) {
        auto data = ExcelParser::getWorksheetData(multiSheetFile, name);
        EXPECT_FALSE(data.empty()) << "Sheet '" << name << "' should have data";
    }
}

TEST_F(ExcelParserTest, GetWorksheetData_IntegerParsing) {
    auto data = ExcelParser::getWorksheetData(singleSheetFile, "Sheet1");
    
    EXPECT_TRUE(data.find("25") != std::string::npos);
    EXPECT_TRUE(data.find("30") != std::string::npos);
    EXPECT_TRUE(data.find("35") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_FloatParsing) {
    auto data = ExcelParser::getWorksheetData(multiSheetFile, "Products");
    
    EXPECT_TRUE(data.find("5999.99") != std::string::npos);
    EXPECT_TRUE(data.find("99.5") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_LargeInteger) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    EXPECT_TRUE(data.find("9999999999") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_NegativeInteger) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    EXPECT_TRUE(data.find("-100") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_ZeroValues) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    EXPECT_FALSE(data.empty());
    
    std::istringstream iss(data);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    
    ASSERT_GE(lines.size(), 4);
    
    bool hasZero = lines[3].find("0") != std::string::npos;
    EXPECT_TRUE(hasZero);
}

TEST_F(ExcelParserTest, GetWorksheetData_SpecialCharacters) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    EXPECT_TRUE(data.find("!@#$%^&*()") != std::string::npos);
}

TEST_F(ExcelParserTest, GetWorksheetData_BooleanTrue) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    std::istringstream iss(data);
    std::string line;
    bool foundTrue = false;
    
    while (std::getline(iss, line)) {
        std::vector<std::string> cells;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            cells.push_back(cell);
        }
        
        if (cells.size() >= 4 && cells[3] == "1") {
            foundTrue = true;
            break;
        }
    }
    
    EXPECT_TRUE(foundTrue);
}

TEST_F(ExcelParserTest, GetWorksheetData_BooleanFalse) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    std::istringstream iss(data);
    std::string line;
    bool foundFalse = false;
    
    while (std::getline(iss, line)) {
        std::vector<std::string> cells;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            cells.push_back(cell);
        }
        
        if (cells.size() >= 4 && cells[3] == "0") {
            foundFalse = true;
            break;
        }
    }
    
    EXPECT_TRUE(foundFalse);
}

TEST_F(ExcelParserTest, GetWorksheetData_EmptyCell) {
    auto data = ExcelParser::getWorksheetData(variousTypesFile, "DataTypes");
    
    std::istringstream iss(data);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    
    ASSERT_GE(lines.size(), 2);
    
    std::stringstream ss(lines[1]);
    std::string cell;
    std::vector<std::string> cells;
    while (std::getline(ss, cell, ',')) {
        cells.push_back(cell);
    }
    
    // OpenXLSX的row.values()不包含完全空的单元格
    // 当E列设置为空XLCellValue()时，OpenXLSX不会将其视为"已使用"的单元格
    // 因此cells.size()只有4个（A-D列），而不是5个
    // 这是OpenXLSX库的设计行为，不是ExcelParser的bug
    ASSERT_GE(cells.size(), 4);
    ASSERT_EQ(cells.size(), 4);
}

}
}

int main(int argc, char** argv) {
    // 初始化脚手架中的日志
    bitelog::bitelog_init();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
