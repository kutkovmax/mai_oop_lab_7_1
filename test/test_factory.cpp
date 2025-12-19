#include "../include/npc/npc_factory.h"
#include "../include/npc/druid.h"
#include "../include/npc/orc.h"
#include "../include/npc/squirrel.h"
#include "../include/geometry/point.h"
#include <gtest/gtest.h>
#include <memory>
#include <fstream>
#include <filesystem>

TEST(NPCFactoryTest, CreateDruid) {
    NPCFactory factory;
    Point position(10, 20);
    
    auto npc = factory.create("Друид", "Друид1", position);
    
    EXPECT_NE(npc, nullptr);
    EXPECT_EQ(npc->get_name(), "Друид1");
    EXPECT_EQ(npc->get_type(), "Друид");
    EXPECT_EQ(npc->get_position().get_x(), 10);
    EXPECT_EQ(npc->get_position().get_y(), 20);
    EXPECT_TRUE(npc->is_alive());
}

TEST(NPCFactoryTest, CreateOrc) {
    NPCFactory factory;
    Point position(5, 15);
    
    auto npc = factory.create("Орк", "Орк1", position);
    
    EXPECT_NE(npc, nullptr);
    EXPECT_EQ(npc->get_name(), "Орк1");
    EXPECT_EQ(npc->get_type(), "Орк");
    EXPECT_EQ(npc->get_position().get_x(), 5);
    EXPECT_EQ(npc->get_position().get_y(), 15);
    EXPECT_TRUE(npc->is_alive());
}

TEST(NPCFactoryTest, CreateSquirrel) {
    NPCFactory factory;
    Point position(30, 40);
    
    auto npc = factory.create("Белка", "Ваня", position);
    
    EXPECT_NE(npc, nullptr);
    EXPECT_EQ(npc->get_name(), "Ваня");
    EXPECT_EQ(npc->get_type(), "Белка");
    EXPECT_EQ(npc->get_position().get_x(), 30);
    EXPECT_EQ(npc->get_position().get_y(), 40);
    EXPECT_TRUE(npc->is_alive());
}

TEST(NPCFactoryTest, CreateInvalidType) {
    NPCFactory factory;
    Point position(0, 0);
    
    EXPECT_THROW({
        factory.create("Король", "Артур", position);
    }, std::invalid_argument);
    
    EXPECT_THROW({
        factory.create("", "Безымянный", position);
    }, std::invalid_argument);
    
    EXPECT_THROW({
        factory.create("Волшебник", "Гендальф", position);
    }, std::invalid_argument);
}

class NPCFactoryFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        createTestFile("valid_npcs.txt", {
            "Гермиона Друид 10 20",
            "ГарриПоттер Орк 30 40", 
            "Рон Белка 50 60",
            "Хагрид Друид 70 80"
        });
        
        createTestFile("mixed_npcs.txt", {
            "Малфой Друид 10 20",
            "Артур Рыцарь 30 40",
            "Белка1 Белка 50 60"
        });
        
        createTestFile("invalid_format.txt", {
            "Северус Друид 10",
            "Дамблдор Орк",
            "Только_имя",
            "Друид 10 20" 
        });
        
        createTestFile("empty.txt", {});
        
        createTestFile("special_names.txt", {
            "Имя_с_подчерками Друид 1 1",
            "Волан-де-Морт Орк 2 2", 
            "ИмяБезПробелов Белка 3 3"
        });
    }
    
    void TearDown() override {
        cleanupFile("valid_npcs.txt");
        cleanupFile("mixed_npcs.txt");
        cleanupFile("invalid_format.txt");
        cleanupFile("empty.txt");
        cleanupFile("special_names.txt");
        cleanupFile("output.txt");
    }
    
    void createTestFile(const std::string& filename, const std::vector<std::string>& lines) {
        std::ofstream file(filename);
        for (const auto& line : lines) {
            file << line << "\n";
        }
    }
    
    void cleanupFile(const std::string& filename) {
        if (std::filesystem::exists(filename)) {
            std::filesystem::remove(filename);
        }
    }
    
    NPCFactory factory;
};

TEST_F(NPCFactoryFileTest, LoadFromFileValid) {
    auto npcs = factory.load_from_file("valid_npcs.txt");
    
    EXPECT_EQ(npcs.size(), 4);
    
    EXPECT_EQ(npcs[0]->get_name(), "Гермиона");
    EXPECT_EQ(npcs[0]->get_type(), "Друид");
    EXPECT_EQ(npcs[0]->get_position().get_x(), 10);
    EXPECT_EQ(npcs[0]->get_position().get_y(), 20);
    
    EXPECT_EQ(npcs[1]->get_name(), "ГарриПоттер");
    EXPECT_EQ(npcs[1]->get_type(), "Орк");
    EXPECT_EQ(npcs[1]->get_position().get_x(), 30);
    EXPECT_EQ(npcs[1]->get_position().get_y(), 40);
    
    EXPECT_EQ(npcs[2]->get_name(), "Рон");
    EXPECT_EQ(npcs[2]->get_type(), "Белка");
    EXPECT_EQ(npcs[2]->get_position().get_x(), 50);
    EXPECT_EQ(npcs[2]->get_position().get_y(), 60);
}

TEST_F(NPCFactoryFileTest, LoadFromFileMixedValidInvalid) {
    testing::internal::CaptureStderr();
    
    auto npcs = factory.load_from_file("mixed_npcs.txt");
    
    std::string output = testing::internal::GetCapturedStderr();
    
    EXPECT_EQ(npcs.size(), 2);
    EXPECT_TRUE(output.find("Ошибка создания NPC") != std::string::npos);
    
    EXPECT_EQ(npcs[0]->get_name(), "Малфой");
    EXPECT_EQ(npcs[0]->get_type(), "Друид");
    EXPECT_EQ(npcs[1]->get_name(), "Белка1");
    EXPECT_EQ(npcs[1]->get_type(), "Белка");
}

TEST_F(NPCFactoryFileTest, LoadFromFileEmpty) {
    auto npcs = factory.load_from_file("empty.txt");
    EXPECT_EQ(npcs.size(), 0);
}

TEST_F(NPCFactoryFileTest, LoadFromFileInvalidFormat) {
    testing::internal::CaptureStderr();
    
    auto npcs = factory.load_from_file("invalid_format.txt");
    
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(npcs.size(), 0);
}

TEST_F(NPCFactoryFileTest, LoadFromFileSpecialNames) {
    auto npcs = factory.load_from_file("special_names.txt");
    
    EXPECT_EQ(npcs.size(), 3);
    
    EXPECT_EQ(npcs[0]->get_name(), "Имя_с_подчерками");
    EXPECT_EQ(npcs[0]->get_type(), "Друид");
    EXPECT_EQ(npcs[0]->get_position().get_x(), 1);
    EXPECT_EQ(npcs[0]->get_position().get_y(), 1);
    
    EXPECT_EQ(npcs[1]->get_name(), "Волан-де-Морт");
    EXPECT_EQ(npcs[1]->get_type(), "Орк");
    EXPECT_EQ(npcs[1]->get_position().get_x(), 2);
    EXPECT_EQ(npcs[1]->get_position().get_y(), 2);
    
    EXPECT_EQ(npcs[2]->get_name(), "ИмяБезПробелов");
    EXPECT_EQ(npcs[2]->get_type(), "Белка");
    EXPECT_EQ(npcs[2]->get_position().get_x(), 3);
    EXPECT_EQ(npcs[2]->get_position().get_y(), 3);
}

TEST_F(NPCFactoryFileTest, LoadFromFileNonExistent) {
    EXPECT_THROW({
        factory.load_from_file("non_existent_file.txt");
    }, std::runtime_error);
}

TEST_F(NPCFactoryFileTest, SaveToFile) {
    std::vector<std::unique_ptr<NPC>> npcs;
    npcs.push_back(factory.create("Друид", "Мерлин", Point(10, 20)));
    npcs.push_back(factory.create("Орк", "Гром'Гор", Point(30, 40)));
    npcs.push_back(factory.create("Белка", "Белка1", Point(50, 60)));
    
    npcs[1]->kill();
    
    factory.save_to_file("output.txt", npcs);
    
    std::ifstream file("output.txt");
    ASSERT_TRUE(file.is_open());
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    
    EXPECT_EQ(lines.size(), 2);
    
    bool found_druid = false;
    bool found_squirrel = false;
    
    for (const auto& saved_line : lines) {
        if (saved_line.find("Мерлин Друид 10 20") != std::string::npos) {
            found_druid = true;
        }
        if (saved_line.find("Белка1 Белка 50 60") != std::string::npos) {
            found_squirrel = true;
        }
    }
    
    EXPECT_TRUE(found_druid);
    EXPECT_TRUE(found_squirrel);
}

TEST_F(NPCFactoryFileTest, SaveToFileAllDead) {
    std::vector<std::unique_ptr<NPC>> npcs;
    npcs.push_back(factory.create("Друид", "Мерлин", Point(10, 20)));
    npcs.push_back(factory.create("Орк", "Гром'Гор", Point(30, 40)));
    
    for (auto& npc : npcs) {
        npc->kill();
    }
    
    factory.save_to_file("output.txt", npcs);
    
    std::ifstream file("output.txt");
    ASSERT_TRUE(file.is_open());
    
    std::string line;
    int line_count = 0;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            line_count++;
        }
    }
    
    EXPECT_EQ(line_count, 0);
}

TEST_F(NPCFactoryFileTest, SaveToFileEmptyVector) {
    std::vector<std::unique_ptr<NPC>> npcs;
    
    factory.save_to_file("output.txt", npcs);
    
    std::ifstream file("output.txt");
    ASSERT_TRUE(file.is_open());
    
    std::string line;
    int line_count = 0;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            line_count++;
        }
    }
    
    EXPECT_EQ(line_count, 0);
}

TEST_F(NPCFactoryFileTest, SaveToFileInvalidPath) {
    std::vector<std::unique_ptr<NPC>> npcs;
    npcs.push_back(factory.create("Друид", "Мерлин", Point(10, 20)));
    
    EXPECT_THROW({
        factory.save_to_file("/invalid/path/npcs.txt", npcs);
    }, std::runtime_error);
}

TEST_F(NPCFactoryFileTest, FullCycle) {
    std::vector<std::unique_ptr<NPC>> original_npcs;
    original_npcs.push_back(factory.create("Друид", "Мерлин", Point(10, 20)));
    original_npcs.push_back(factory.create("Орк", "Гром'Гор", Point(30, 40)));
    original_npcs.push_back(factory.create("Белка", "Белка1", Point(50, 60)));
    
    factory.save_to_file("output.txt", original_npcs);
    
    auto loaded_npcs = factory.load_from_file("output.txt");
    
    EXPECT_EQ(loaded_npcs.size(), original_npcs.size());
    
    for (size_t i = 0; i < loaded_npcs.size(); ++i) {
        EXPECT_EQ(loaded_npcs[i]->get_name(), original_npcs[i]->get_name());
        EXPECT_EQ(loaded_npcs[i]->get_type(), original_npcs[i]->get_type());
        EXPECT_EQ(loaded_npcs[i]->get_position().get_x(), original_npcs[i]->get_position().get_x());
        EXPECT_EQ(loaded_npcs[i]->get_position().get_y(), original_npcs[i]->get_position().get_y());
        EXPECT_TRUE(loaded_npcs[i]->is_alive());
    }
}

TEST(NPCFactoryEdgeCasesTest, SpecialNamesAndPositions) {
    NPCFactory factory;
    
    auto npc1 = factory.create("Друид", "", Point(0, 0));
    EXPECT_EQ(npc1->get_name(), "");
    EXPECT_EQ(npc1->get_type(), "Друид");
    
    auto npc2 = factory.create("Орк", "Громовой-Молот", Point(1, 1));
    EXPECT_EQ(npc2->get_name(), "Громовой-Молот");
    EXPECT_EQ(npc2->get_type(), "Орк");
    
    auto npc3 = factory.create("Белка", "Белка1", Point(-10, -20));
    EXPECT_EQ(npc3->get_position().get_x(), -10);
    EXPECT_EQ(npc3->get_position().get_y(), -20);
    
    auto npc4 = factory.create("Друид", "Друид", Point(1000, 2000));
    EXPECT_EQ(npc4->get_position().get_x(), 1000);
    EXPECT_EQ(npc4->get_position().get_y(), 2000);
    
    auto npc5 = factory.create("Орк", "Орк", Point(0, 0));
    EXPECT_EQ(npc5->get_position().get_x(), 0);
    EXPECT_EQ(npc5->get_position().get_y(), 0);
}

TEST(NPCFactoryTest, CreateMultipleNPCs) {
    NPCFactory factory;
    
    auto druid = factory.create("Друид", "Друид1", Point(1, 1));
    auto orc = factory.create("Орк", "Орк1", Point(2, 2));
    auto squirrel = factory.create("Белка", "Белка1", Point(3, 3));
    
    EXPECT_EQ(druid->get_type(), "Друид");
    EXPECT_EQ(orc->get_type(), "Орк");
    EXPECT_EQ(squirrel->get_type(), "Белка");
}

