#include <jni.h>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <algorithm>

// Define 64 distinct 8x8 pixel glyphs
// Each glyph is 8 rows, each row is 1 byte (8 bits) representing pixel columns
const int NUM_GLYPHS = 64;
const unsigned char GLYPHS[NUM_GLYPHS][8] = {
    // Digits '0' - '9'
    {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00}, // 0
    {0x18, 0x1C, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00}, // 1
    {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00}, // 2
    {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00}, // 3
    {0x0C, 0x1C, 0x2C, 0x4C, 0x7E, 0x0C, 0x0C, 0x00}, // 4
    {0x7E, 0x40, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00}, // 5
    {0x3C, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x3C, 0x00}, // 6
    {0x7E, 0x66, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x00}, // 7
    {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00}, // 8
    {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00}, // 9

    // Uppercase Letters 'A' - 'Z'
    {0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00}, // A
    {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00}, // B
    {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00}, // C
    {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00}, // D
    {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00}, // E
    {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00}, // F
    {0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3E, 0x00}, // G
    {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00}, // H
    {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00}, // I
    {0x1E, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3C, 0x00}, // J
    {0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00}, // K
    {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00}, // L
    {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00}, // M
    {0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00}, // N
    {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}, // O
    {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00}, // P
    {0x3C, 0x66, 0x66, 0x66, 0x6E, 0x6C, 0x3E, 0x02}, // Q
    {0x7C, 0x66, 0x66, 0x7C, 0x78, 0x6C, 0x66, 0x00}, // R
    {0x3E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00}, // S
    {0x7E, 0x5A, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00}, // T
    {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}, // U
    {0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00}, // V
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // W
    {0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00}, // X
    {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00}, // Y
    {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00}, // Z

    // Alphanumeric/Katakana-like Symbols
    {0x3C, 0x18, 0x3C, 0x66, 0x66, 0x3C, 0x18, 0x3C}, 
    {0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18}, 
    {0x00, 0x3E, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00}, 
    {0x10, 0x30, 0x7F, 0x30, 0x10, 0x00, 0x7F, 0x00}, 
    {0x08, 0x1C, 0x36, 0x63, 0x7F, 0x63, 0x63, 0x00}, 
    {0x7C, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x7C, 0x00}, 
    {0x1C, 0x22, 0x40, 0x3C, 0x02, 0x22, 0x1C, 0x00}, 
    {0x08, 0x18, 0x28, 0x48, 0x7F, 0x08, 0x08, 0x00}, 
    {0x44, 0x28, 0x10, 0x28, 0x44, 0x28, 0x10, 0x00}, 
    {0x00, 0x10, 0x38, 0x54, 0x10, 0x10, 0x10, 0x00}, 
    {0x3C, 0x42, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00}, 
    {0x1C, 0x08, 0x08, 0x1C, 0x22, 0x22, 0x1C, 0x00}, 
    {0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00}, 
    {0x42, 0x81, 0x00, 0xFF, 0x00, 0x81, 0x42, 0x00}, 
    {0x1C, 0x22, 0x5A, 0x5A, 0x22, 0x1C, 0x00, 0x00}, 
    {0x18, 0x18, 0x7E, 0x18, 0x18, 0x18, 0x7E, 0x18}, 
    {0x00, 0x66, 0x00, 0x7E, 0x00, 0x66, 0x00, 0x00}, 
    {0x3C, 0x5A, 0x66, 0x66, 0x5A, 0x3C, 0x00, 0x00}, 
    {0x24, 0x7E, 0x24, 0x24, 0x7E, 0x24, 0x00, 0x00}, 
    {0x1C, 0x22, 0x40, 0x40, 0x22, 0x1C, 0x08, 0x00}, 
    {0x08, 0x1C, 0x2A, 0x08, 0x08, 0x08, 0x08, 0x00}, 
    {0x10, 0x38, 0x54, 0x10, 0x38, 0x54, 0x10, 0x00}, 
    {0x3C, 0x46, 0x42, 0x3C, 0x42, 0x62, 0x3C, 0x00}, 
    {0x18, 0x24, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x00}, 
    {0x1C, 0x22, 0x22, 0x1C, 0x22, 0x22, 0x1C, 0x00}, 
    {0x7C, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x7C, 0x00}, 
    {0x0C, 0x12, 0x22, 0x42, 0x7E, 0x02, 0x02, 0x00}, 
    {0x18, 0x3C, 0x24, 0x24, 0x3C, 0x18, 0x18, 0x00}
};

class MatrixEngine {
public:
    int width = 0;
    int height = 0;
    int cols = 0;

    struct Column {
        float headY;          // Y position in pixels
        float speed;          // Speed in pixels/second
        std::vector<int> glyphIndices; // glyph index for each cell in this column
        std::vector<float> switchTimers; // time remaining before swapping a cell's glyph
    };

    std::vector<Column> columns;
    std::chrono::steady_clock::time_point lastTime;
    bool hasTime = false;

    void resize(int w, int h) {
        if (width == w && height == h) return;
        width = w;
        height = h;

        int cellWidth = 8; // font cell width
        cols = width / cellWidth;
        columns.clear();
        columns.resize(cols);

        int rows = height / 8; // font cell height
        if (rows < 10) rows = 10;

        for (int i = 0; i < cols; ++i) {
            resetColumn(i, true);
        }
    }

    void resetColumn(int col, bool initRandomY) {
        int rows = height / 8;
        if (rows < 10) rows = 10;

        columns[col].speed = 80.0f + (rand() % 160); // 80 to 240 pixels per second
        if (initRandomY) {
            columns[col].headY = -(rand() % std::max(1, height));
        } else {
            // Start column above screen by a random offset
            columns[col].headY = -(rand() % 160) - 24.0f;
        }

        columns[col].glyphIndices.resize(rows);
        columns[col].switchTimers.resize(rows);
        for (int r = 0; r < rows; ++r) {
            columns[col].glyphIndices[r] = rand() % NUM_GLYPHS;
            columns[col].switchTimers[r] = 0.2f + ((rand() % 100) / 50.0f); // 0.2s to 2.2s
        }
    }

    void update(float dt) {
        int rows = height / 8;
        if (rows <= 0) return;

        for (int i = 0; i < cols; ++i) {
            Column& col = columns[i];
            col.headY += col.speed * dt;

            // If trail is fully off screen, reset the column
            int trailLength = 22;
            if (col.headY - (trailLength * 8) > height) {
                resetColumn(i, false);
            }

            // Periodically flicker/switch glyphs along the column to match the Matrix style
            for (int r = 0; r < rows; ++r) {
                if (r < (int)col.switchTimers.size()) {
                    col.switchTimers[r] -= dt;
                    if (col.switchTimers[r] <= 0.0f) {
                        col.glyphIndices[r] = rand() % NUM_GLYPHS;
                        col.switchTimers[r] = 0.3f + ((rand() % 100) / 40.0f); // 0.3s to 2.8s
                    }
                }
            }
        }
    }

    void render(uint32_t* pixels) {
        // Clear background to full transparent black (0x00000000) or solid black (0xFF000000)
        // Solid black is safer for simple Bitmap representations: 0xFF000000
        std::fill_n(pixels, width * height, 0xFF000000);

        int cellWidth = 8;
        int cellHeight = 8;

        for (int colIdx = 0; colIdx < cols; ++colIdx) {
            const Column& col = columns[colIdx];
            int xStart = colIdx * cellWidth;

            // Determine which row is currently the "head" of the drop
            int headRow = (int)(col.headY / cellHeight);

            // Draw a fading trail of glyphs upwards from the head row
            int maxTrail = 20;
            for (int r = headRow; r >= 0 && r >= headRow - maxTrail; --r) {
                if (r >= (int)col.glyphIndices.size()) continue;

                int yStart = r * cellHeight;
                if (yStart + cellHeight > height) continue;

                int age = headRow - r; // 0 is head
                uint32_t color = 0;

                if (age == 0) {
                    // Head is bright white
                    color = 0xFFFFFFFF;
                } else if (age < 3) {
                    // Bright green fading in
                    color = 0xFF90FF90;
                } else {
                    // Green trail decays and dims
                    int green = 255 - (age - 3) * 14;
                    if (green < 15) green = 15;
                    color = 0xFF000000 | (green << 8); // ARGB: Alpha FF, Red 00, Green green, Blue 00
                }

                int glyphIdx = col.glyphIndices[r];
                drawGlyph(pixels, xStart, yStart, glyphIdx, color);
            }
        }
    }

    void drawGlyph(uint32_t* pixels, int x, int y, int glyphIdx, uint32_t color) {
        if (glyphIdx < 0 || glyphIdx >= NUM_GLYPHS) return;
        const unsigned char* bitmap = GLYPHS[glyphIdx];

        for (int row = 0; row < 8; ++row) {
            int targetY = y + row;
            if (targetY < 0 || targetY >= height) continue;

            unsigned char b = bitmap[row];
            for (int col = 0; col < 8; ++col) {
                int targetX = x + col;
                if (targetX < 0 || targetX >= width) continue;

                // Check bits from MSB (left) to LSB (right)
                if ((b >> (7 - col)) & 1) {
                    pixels[targetY * width + targetX] = color;
                }
            }
        }
    }

    void tick() {
        auto now = std::chrono::steady_clock::now();
        float dt = 0.016f; // Default delta time
        if (hasTime) {
            std::chrono::duration<float> elapsed = now - lastTime;
            dt = elapsed.count();
        } else {
            hasTime = true;
        }
        lastTime = now;

        // Cap dt to prevent massive jumps on pause/resume or hiccups
        if (dt > 0.1f) dt = 0.1f;

        update(dt);
    }
};

// Global MatrixEngine instance
static MatrixEngine g_engine;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_matrixrain_MatrixNativeBridge_nativeUpdateAndRender(
    JNIEnv* env,
    jobject thiz,
    jintArray pixels,
    jint width,
    jint height
) {
    g_engine.resize(width, height);

    // Lock the Java pixel array to obtain raw pointer
    jint* pixelArray = env->GetIntArrayElements(pixels, nullptr);
    if (!pixelArray) return;

    // Tick/update column particle physics
    g_engine.tick();

    // Render characters into pixel array
    g_engine.render(reinterpret_cast<uint32_t*>(pixelArray));

    // Release and copy changes back to Java memory
    env->ReleaseIntArrayElements(pixels, pixelArray, 0);
}