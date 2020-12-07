#pragma once

#include <glad/glad.h>

// Texture2D ������ � ���� �������� � ����� ����������� � ����������������.
// �� ����� �������� ��������� ������� ��� �������� ����������.
class Texture2D
{
public:
    // �������� ID ������� ��������, ������������ ��� ���� �������� � ���������� ���������
    unsigned int ID;
    // ������� ����������� �����������
    unsigned int Width, Height; // ������ � ������ (� ��������) ������������ �����������
    // ������ ��������
    unsigned int Internal_Format; // ���������� ������ ��������
    unsigned int Image_Format; // ������ ������������ �����������
    // ������������ ��������
    unsigned int Wrap_S; // ����� ��������� �� ��� S
    unsigned int Wrap_T; // ����� ��������� �� ��� T
    unsigned int Filter_Min; // ����� ����������, ���� �������� �������� < �������� ������
    unsigned int Filter_Max; //  ����� ����������, ���� �������� �������� > �������� ������
    // ����������� (������������� �������� ��-��������� ��������)
    Texture2D();
    // ���������� �������� �� �����������
    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    // ��������� �������� � ���� �������� ��������� GL_TEXTURE_2D ����������� �������
    void Bind() const;
};