#pragma once

#include "chunk.h"
#include <array>
#include <unordered_set>

using namespace std;

#define BLK_UVX * 0.0625f
#define BLK_UVY * 0.0625f
#define BLK_UV * 0.0625f

enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, WATER, SNOW, LAVA, BEDROCK, NONINIT , SAND, SNOWGRASS, WOOD, LEAF, ICE, CACTI, REDFLOWER, TARES, DEADBUSH, CUCURBIT
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

const static unordered_map<BlockType, unordered_map<Direction, glm::vec2, EnumHash>, EnumHash> blockFaceUVs {
    {GRASS, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(3.f BLK_UVX,15.f BLK_UVY)},
                                                          {XNEG, glm::vec2(3.f BLK_UVX,15.f BLK_UVY)},
                                                          {YPOS, glm::vec2(8.f BLK_UVX,13.f BLK_UVY)},
                                                          {YNEG, glm::vec2(2.f BLK_UVX,15.f BLK_UVY)},
                                                          {ZPOS, glm::vec2(3.f BLK_UVX,15.f BLK_UVY)},
                                                          {ZNEG, glm::vec2(3.f BLK_UVX,15.f BLK_UVY)}}},
    {TARES, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(7.f BLK_UVX,13.f BLK_UVY)},
                                                          {XNEG, glm::vec2(7.f BLK_UVX,13.f BLK_UVY)},
                                                          {YPOS, glm::vec2(4.f BLK_UVX,4.f BLK_UVY)},
                                                          {YNEG, glm::vec2(4.f BLK_UVX,4.f BLK_UVY)},
                                                          {ZPOS, glm::vec2(7.f BLK_UVX,13.f BLK_UVY)},
                                                          {ZNEG, glm::vec2(7.f BLK_UVX,13.f BLK_UVY)}}},
    {DEADBUSH, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(7.f BLK_UVX,12.f BLK_UVY)},
                                                          {XNEG, glm::vec2(7.f BLK_UVX,12.f BLK_UVY)},
                                                          {YPOS, glm::vec2(4.f BLK_UVX,4.f BLK_UVY)},
                                                          {YNEG, glm::vec2(4.f BLK_UVX,4.f BLK_UVY)},
                                                          {ZPOS, glm::vec2(7.f BLK_UVX,12.f BLK_UVY)},
                                                          {ZNEG, glm::vec2(7.f BLK_UVX,12.f BLK_UVY)}}},
    {DIRT, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f BLK_UVX,15.f BLK_UVY)},
                                                          {XNEG, glm::vec2(2.f BLK_UVX,15.f BLK_UVY)},
                                                          {YPOS, glm::vec2(2.f BLK_UVX,15.f BLK_UVY)},
                                                          {YNEG, glm::vec2(2.f BLK_UVX,15.f BLK_UVY)},
                                                          {ZPOS, glm::vec2(2.f BLK_UVX,15.f BLK_UVY)},
                                                          {ZNEG, glm::vec2(2.f BLK_UVX,15.f BLK_UVY)}}},
    {STONE, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(1.f BLK_UVX,15.f BLK_UVY)},
                                                          {XNEG, glm::vec2(1.f BLK_UVX,15.f BLK_UVY)},
                                                          {YPOS, glm::vec2(1.f BLK_UVX,15.f BLK_UVY)},
                                                          {YNEG, glm::vec2(1.f BLK_UVX,15.f BLK_UVY)},
                                                          {ZPOS, glm::vec2(1.f BLK_UVX,15.f BLK_UVY)},
                                                          {ZNEG, glm::vec2(1.f BLK_UVX,15.f BLK_UVY)}}},
    {BEDROCK, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(1.f BLK_UVX,14.f BLK_UVY)},
                                                          {XNEG, glm::vec2(1.f BLK_UVX,14.f BLK_UVY)},
                                                          {YPOS, glm::vec2(1.f BLK_UVX,14.f BLK_UVY)},
                                                          {YNEG, glm::vec2(1.f BLK_UVX,14.f BLK_UVY)},
                                                          {ZPOS, glm::vec2(1.f BLK_UVX,14.f BLK_UVY)},
                                                          {ZNEG, glm::vec2(1.f BLK_UVX,14.f BLK_UVY)}}},
    {LAVA, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(13.f BLK_UVX,1.f BLK_UVY)},
                                                            {XNEG, glm::vec2(13.f BLK_UVX,1.f BLK_UVY)},
                                                            {YPOS, glm::vec2(13.f BLK_UVX,1.f BLK_UVY)},
                                                            {YNEG, glm::vec2(13.f BLK_UVX,1.f BLK_UVY)},
                                                            {ZPOS, glm::vec2(13.f BLK_UVX,1.f BLK_UVY)},
                                                            {ZNEG, glm::vec2(13.f BLK_UVX,1.f BLK_UVY)}}},
    {WATER, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(13.f BLK_UVX,3.f BLK_UVY)},
                                                            {XNEG, glm::vec2(13.f BLK_UVX,3.f BLK_UVY)},
                                                            {YPOS, glm::vec2(13.f BLK_UVX,3.f BLK_UVY)},
                                                            {YNEG, glm::vec2(13.f BLK_UVX,3.f BLK_UVY)},
                                                            {ZPOS, glm::vec2(13.f BLK_UVX,3.f BLK_UVY)},
                                                            {ZNEG, glm::vec2(13.f BLK_UVX,3.f BLK_UVY)}}},
    {SNOW, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f BLK_UVX,11.f BLK_UVY)},
                                                          {XNEG, glm::vec2(2.f BLK_UVX,11.f BLK_UVY)},
                                                          {YPOS, glm::vec2(2.f BLK_UVX,11.f BLK_UVY)},
                                                          {YNEG, glm::vec2(2.f BLK_UVX,11.f BLK_UVY)},
                                                          {ZPOS, glm::vec2(2.f BLK_UVX,11.f BLK_UVY)},
                                                          {ZNEG, glm::vec2(2.f BLK_UVX,11.f BLK_UVY)}}},
    {SAND, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f BLK_UVX,14.f BLK_UVY)},
                                                         {XNEG, glm::vec2(2.f BLK_UVX,14.f BLK_UVY)},
                                                         {YPOS, glm::vec2(2.f BLK_UVX,14.f BLK_UVY)},
                                                         {YNEG, glm::vec2(2.f BLK_UVX,14.f BLK_UVY)},
                                                         {ZPOS, glm::vec2(2.f BLK_UVX,14.f BLK_UVY)},
                                                         {ZNEG, glm::vec2(2.f BLK_UVX,14.f BLK_UVY)}}},
    {SNOWGRASS, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(4.f BLK_UVX,11.f BLK_UVY)},
                                                         {XNEG, glm::vec2(4.f BLK_UVX,11.f BLK_UVY)},
                                                         {YPOS, glm::vec2(2.f BLK_UVX,11.f BLK_UVY)},
                                                         {YNEG, glm::vec2(4.f BLK_UVX,11.f BLK_UVY)},
                                                         {ZPOS, glm::vec2(4.f BLK_UVX,11.f BLK_UVY)},
                                                         {ZNEG, glm::vec2(4.f BLK_UVX,11.f BLK_UVY)}}},
    {CACTI, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(6.f BLK_UVX,11.f BLK_UVY)},
                                                          {XNEG, glm::vec2(6.f BLK_UVX,11.f BLK_UVY)},
                                                          {YPOS, glm::vec2(5.f BLK_UVX,11.f BLK_UVY)},
                                                          {YNEG, glm::vec2(7.f BLK_UVX,11.f BLK_UVY)},
                                                          {ZPOS, glm::vec2(6.f BLK_UVX,11.f BLK_UVY)},
                                                          {ZNEG, glm::vec2(6.f BLK_UVX,11.f BLK_UVY)}}},
    {ICE, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(3.f BLK_UVX,11.f BLK_UVY)},
                                                          {XNEG, glm::vec2(3.f BLK_UVX,11.f BLK_UVY)},
                                                          {YPOS, glm::vec2(3.f BLK_UVX,11.f BLK_UVY)},
                                                          {YNEG, glm::vec2(3.f BLK_UVX,11.f BLK_UVY)},
                                                          {ZPOS, glm::vec2(3.f BLK_UVX,11.f BLK_UVY)},
                                                          {ZNEG, glm::vec2(3.f BLK_UVX,11.f BLK_UVY)}}},
    {WOOD, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(4.f BLK_UVX,14.f BLK_UVY)},
                                                         {XNEG, glm::vec2(4.f BLK_UVX,14.f BLK_UVY)},
                                                         {YPOS, glm::vec2(5.f BLK_UVX,14.f BLK_UVY)},
                                                         {YNEG, glm::vec2(5.f BLK_UVX,14.f BLK_UVY)},
                                                         {ZPOS, glm::vec2(4.f BLK_UVX,14.f BLK_UVY)},
                                                         {ZNEG, glm::vec2(4.f BLK_UVX,14.f BLK_UVY)}}},
    {LEAF, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(4.f BLK_UVX,12.f BLK_UVY)},
                                                         {XNEG, glm::vec2(4.f BLK_UVX,12.f BLK_UVY)},
                                                         {YPOS, glm::vec2(4.f BLK_UVX,12.f BLK_UVY)},
                                                         {YNEG, glm::vec2(4.f BLK_UVX,12.f BLK_UVY)},
                                                         {ZPOS, glm::vec2(4.f BLK_UVX,12.f BLK_UVY)},
                                                         {ZNEG, glm::vec2(4.f BLK_UVX,12.f BLK_UVY)}}},
    {REDFLOWER, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(12.f BLK_UVX,15.f BLK_UVY)},
                                                         {XNEG, glm::vec2(12.f BLK_UVX,15.f BLK_UVY)},
                                                         {YPOS, glm::vec2(4.f BLK_UVX,4.f BLK_UVY)},
                                                         {YNEG, glm::vec2(4.f BLK_UVX,4.f BLK_UVY)},
                                                         {ZPOS, glm::vec2(12.f BLK_UVX,15.f BLK_UVY)},
                                                         {ZNEG, glm::vec2(12.f BLK_UVX,15.f BLK_UVY)}}},
    {CUCURBIT, unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(8.f BLK_UVX,8.f BLK_UVY)},
                                                              {XNEG, glm::vec2(6.f BLK_UVX,8.f BLK_UVY)},
                                                              {YPOS, glm::vec2(6.f BLK_UVX,9.f BLK_UVY)},
                                                              {YNEG, glm::vec2(6.f BLK_UVX,8.f BLK_UVY)},
                                                              {ZPOS, glm::vec2(6.f BLK_UVX,8.f BLK_UVY)},
                                                              {ZNEG, glm::vec2(6.f BLK_UVX,8.f BLK_UVY)}}},
    };

