#include <iostream>
#include <fstream>
#include <string.h>
#pragma warning(disable : 4996)

const size_t smallFilterSize = 3;
const size_t largeFilterSize = 5;

const size_t image1Rows = 1500;
const size_t image1Cols = 2100;
char image1FileName[50] = "./Image1.raw";
char image1Horizontal_3x3[50] = "./Image1Horizontal_3x3.raw";
char image1Vertical_3x3[50] = "./Image1Vertical_3x3.raw";
char image1Horizontal_5x5[50] = "./Image1Horizontal_5x5.raw";
char image1Vertical_5x5[50] = "./Image1Vertical_5x5.raw";

const size_t image2Rows = 750;
const size_t image2Cols = 500;
char image2FileName[50] = "./Image2.raw";
char image2Horizontal_3x3[50] = "./Image2Horizontal_3x3.raw";
char image2Vertical_3x3[50] = "./Image2Vertical_3x3.raw";
char image2Horizontal_5x5[50] = "./Image2Horizontal_5x5.raw";
char image2Vertical_5x5[50] = "./Image2Vertical_5x5.raw";

const size_t image3Rows = 331;
const size_t image3Cols = 550;
char image3FileName[50] = "./Image3.raw";
char image3Horizontal_3x3[50] = "./Image3Horizontal_3x3.raw";
char image3Vertical_3x3[50] = "./Image3Vertical_3x3.raw";
char image3Horizontal_5x5[50] = "./Image3Horizontal_5x5.raw";
char image3Vertical_5x5[50] = "./Image3Vertical_5x5.raw";

void read(int** image, char* fileName, size_t rows, size_t cols) {
    FILE* inputFile = fopen(fileName, "rb");

    if (inputFile) {
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                image[i][j] = fgetc(inputFile);
            }
        }
        fclose(inputFile);
    }
}

void write(int** image, char* fileName, size_t rows, size_t cols) {
    FILE* outputFile = fopen(fileName, "wb");

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            fputc(image[i][j], outputFile);
        }
    }

    fclose(outputFile);
}

int** CreateTwoDimArray(size_t rows, size_t cols) {
    int** twoDimArray = new int * [rows];
    for (size_t i = 0; i < rows; i++) {
        twoDimArray[i] = new int[cols];
        for (size_t j = 0; j < cols; j++) {
            twoDimArray[i][j] = 0;
        }
    }
    return twoDimArray;
}

void Fill3x3Filter(int** h_filter, int** v_filter) {
    int diffOperator[3] = { -1, 0, 1 };
    int weight[3] = { 1, 2, 1 };

    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            h_filter[i][j] = weight[i] * diffOperator[j];
            v_filter[i][j] = weight[j] * diffOperator[i];
        }
    }
}

void Fill5x5Filter(int** h_filter, int** v_filter) {
    int diffOperator[5] = { -1, -2, 0, 2, 1 };
    int weight[5] = { 1, 2, 4, 2, 1 };

    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 5; j++) {
            h_filter[i][j] = weight[i] * diffOperator[j];
            v_filter[i][j] = weight[j] * diffOperator[i];
        }
    }
}

void SetEdgesToZeros(int** image, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; i++) {
        image[i][0] = 0;
        image[i][cols - 1] = 0;
    }

    for (size_t j = 0; j < cols; j++) {
        image[0][j] = 0;
        image[rows - 1][j] = 0;
    }
}

void ApplySobelFilter(int** origImage, int** h_sobelImage, int** v_sobelImage, int** h_filter, int** v_filter, size_t rows, size_t cols, size_t filterSize) {
    int filterDiff = ((int)filterSize - 1) / 2;
    for (size_t i = filterDiff; i < rows - filterDiff; i++) {
        for (size_t j = filterDiff; j < cols - filterDiff; j++) {
            int h_sum = 0;
            int v_sum = 0;
            for (size_t f_row = 0; f_row < filterSize; f_row++) {
                for (size_t f_col = 0; f_col < filterSize; f_col++) {
                    int correlatedRow = i - filterDiff + f_row;
                    int correlatedCol = j - filterDiff + f_col;
                    h_sum += h_filter[f_row][f_col] * origImage[correlatedRow][correlatedCol];
                    v_sum += v_filter[f_row][f_col] * origImage[correlatedRow][correlatedCol];
                }
            }
            h_sobelImage[i][j] = h_sum;
            v_sobelImage[i][j] = v_sum;
        }
    }
}

void NormalizeImage(int** image, size_t rows, size_t cols) {
    int min = 0;
    int max = 0;
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            if (image[i][j] < min) {
                min = image[i][j];
            }
            else if (image[i][j] > max) {
                max = image[i][j];
            }
        }
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            image[i][j] = (float (image[i][j] - min) / (max - min)) * 255;
        }
    }
}

void DetectEdges(char* inputFileName, char* h_3x3OutputFileName, char* v_3x3OutputFileName, char* h_5x5OutputFileName, char* v_5x5OutputFileName, size_t rows, size_t cols) {
    int** h_filter3x3 = CreateTwoDimArray(smallFilterSize, smallFilterSize);
    int** v_filter3x3 = CreateTwoDimArray(smallFilterSize, smallFilterSize);
    Fill3x3Filter(h_filter3x3, v_filter3x3);

    int** h_filter5x5 = CreateTwoDimArray(largeFilterSize, largeFilterSize);
    int** v_filter5x5 = CreateTwoDimArray(largeFilterSize, largeFilterSize);
    Fill5x5Filter(h_filter5x5, v_filter5x5);

    int** image = CreateTwoDimArray(rows, cols);
    int** image_horizontal = CreateTwoDimArray(rows, cols);
    int** image_vertical = CreateTwoDimArray(rows, cols);

    read(image, inputFileName, rows, cols);
    SetEdgesToZeros(image, rows, cols);

    ApplySobelFilter(image, image_horizontal, image_vertical, h_filter3x3, v_filter3x3, rows, cols, smallFilterSize);
    NormalizeImage(image_horizontal, rows, cols);
    NormalizeImage(image_vertical, rows, cols);

    write(image_horizontal, h_3x3OutputFileName, rows, cols);
    write(image_vertical, v_3x3OutputFileName, rows, cols);

    ApplySobelFilter(image, image_horizontal, image_vertical, h_filter5x5, v_filter5x5, rows, cols, largeFilterSize);
    NormalizeImage(image_horizontal, rows, cols);
    NormalizeImage(image_vertical, rows, cols);

    write(image_horizontal, h_5x5OutputFileName, rows, cols);
    write(image_vertical, v_5x5OutputFileName, rows, cols);
}

int main()
{
    DetectEdges(image1FileName, image1Horizontal_3x3, image1Vertical_3x3, image1Horizontal_5x5, image1Vertical_5x5, image1Rows, image1Cols);
    DetectEdges(image2FileName, image2Horizontal_3x3, image2Vertical_3x3, image2Horizontal_5x5, image2Vertical_5x5, image2Rows, image2Cols);
    DetectEdges(image3FileName, image3Horizontal_3x3, image3Vertical_3x3, image3Horizontal_5x5, image3Vertical_5x5, image3Rows, image3Cols);

    return 0;
}
