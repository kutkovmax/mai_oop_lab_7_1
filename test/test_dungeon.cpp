#include "../include/dungeon/dungeon.h"
#include "../include/geometry/point.h"
#include <gtest/gtest.h>
#include <memory>
#include <fstream>

class DungeonEditorTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::remove("log.txt");
        std::remove("test_save.txt");
    }
    
    void TearDown() override {
        std::remove("log.txt");
        std::remove("test_save.txt");
    }
    
    DungeonEditor editor;
};

// Базовые тесты
TEST_F(DungeonEditorTest, AddNPCValid) {
    editor.add_npc("Друид", "Мерлин", 10, 20);
    
    EXPECT_EQ(editor.get_alive_count(), 1);
    EXPECT_TRUE(editor.is_name_exists("Мерлин"));
}

TEST_F(DungeonEditorTest, AddNPCInvalidType) {
    editor.add_npc("Рыцарь", "Артур", 10, 20);
    
    EXPECT_EQ(editor.get_alive_count(), 0);
    EXPECT_FALSE(editor.is_name_exists("Артур"));
}

TEST_F(DungeonEditorTest, AddMultipleNPCs) {
    editor.add_npc("Друид", "Мерлин", 10, 20);
    editor.add_npc("Орк", "Гром'Гор", 30, 40);
    editor.add_npc("Белка", "Белка1", 50, 60);
    
    EXPECT_EQ(editor.get_alive_count(), 3);
}

TEST_F(DungeonEditorTest, IsNameExists) {
    editor.add_npc("Друид", "Мерлин", 10, 20);
    
    EXPECT_TRUE(editor.is_name_exists("Мерлин"));
    EXPECT_FALSE(editor.is_name_exists("Несуществующий"));
}

TEST_F(DungeonEditorTest, PrintAll) {
    editor.add_npc("Друид", "Мерлин", 10, 20);
    editor.add_npc("Орк", "Гром'Гор", 30, 40);
    
    EXPECT_NO_THROW({
        editor.print_all();
    });
}

TEST_F(DungeonEditorTest, GetAliveCount) {
    EXPECT_EQ(editor.get_alive_count(), 0);
    
    editor.add_npc("Друид", "Мерлин", 10, 20);
    EXPECT_EQ(editor.get_alive_count(), 1);
    
    editor.add_npc("Орк", "Орк", 20, 30);
    EXPECT_EQ(editor.get_alive_count(), 2);
}

TEST_F(DungeonEditorTest, AddDifferentTypes) {
    editor.add_npc("Друид", "Друид1", 0, 0);
    editor.add_npc("Орк", "Орк1", 1, 1);
    editor.add_npc("Белка", "Белка1", 2, 2);
    
    EXPECT_EQ(editor.get_alive_count(), 3);
}

TEST_F(DungeonEditorTest, AddNPCWithNegativeCoordinates) {
    editor.add_npc("Друид", "Мерлин", -10, -20);
    
    EXPECT_EQ(editor.get_alive_count(), 1);
    EXPECT_TRUE(editor.is_name_exists("Мерлин"));
}

TEST_F(DungeonEditorTest, AddNPCWithZeroCoordinates) {
    editor.add_npc("Орк", "Орк", 0, 0);
    
    EXPECT_EQ(editor.get_alive_count(), 1);
    EXPECT_TRUE(editor.is_name_exists("Орк"));
}

// Файловые тесты
TEST_F(DungeonEditorTest, SaveAndLoad) {
    editor.add_npc("Друид", "Мерлин", 10, 20);
    editor.add_npc("Орк", "Гром'Гор", 30, 40);
    
    testing::internal::CaptureStdout();
    editor.save_to_file("test_save.txt");
    std::string save_output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(save_output.find("Данные сохранены") != std::string::npos);
    
    std::ifstream file("test_save.txt");
    EXPECT_TRUE(file.is_open());
    file.close();
    
    DungeonEditor new_editor;
    testing::internal::CaptureStdout();
    new_editor.load_from_file("test_save.txt");
    std::string load_output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(load_output.find("Данные загружены") != std::string::npos);
    
    EXPECT_EQ(new_editor.get_alive_count(), 2);
}

TEST_F(DungeonEditorTest, SaveToInvalidPath) {
    testing::internal::CaptureStderr();
    
    editor.save_to_file("/invalid/path/file.txt");
    std::string output = testing::internal::GetCapturedStderr();
    
    EXPECT_TRUE(output.find("Ошибка при сохранении") != std::string::npos);
}

TEST_F(DungeonEditorTest, LoadFromNonExistentFile) {
    testing::internal::CaptureStderr();
    
    editor.load_from_file("non_existent_file.txt");
    std::string output = testing::internal::GetCapturedStderr();
    
    EXPECT_TRUE(output.find("Ошибка при загрузке") != std::string::npos);
}

