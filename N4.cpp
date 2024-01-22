#include <iostream>
#include <array>
#include <limits>
#include <cmath>

const size_t size = 173;

using Element = std::array<bool, size>;

void createZeroElement(Element& coefficients) {
    coefficients.fill(false);
}

void createCustomElement(Element& coefficients, const std::array<size_t, size>& coefArray, size_t coefCount) {
    coefficients.fill(false);

    for (size_t i = 0; i < coefCount; ++i) {
        if (coefArray[i] < size) {
            coefficients[coefArray[i]] = true;
        }
    }
}

void printElement(const Element& coefficients) {
    for (size_t i = 0; i < size; ++i) {
        std::cout << coefficients[i];
    }
    std::cout << std::endl;
}

void toCoefficientArray(const Element& coefficients, std::array<size_t, size>& result, size_t& arraySize) {
    arraySize = 0;
    for (size_t i = 0; i < size && arraySize < size; ++i) {
        if (coefficients[i]) {
            result[arraySize++] = i;
        }
    }
}

void printCoef(const std::array<size_t, size>& result, size_t arraySize) {
    for (size_t i = 0; i < arraySize; ++i) {
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;
}

// ������� ��� ��������, �� �� �������� ������ ���������� ����. (����������� ��. �� + )
bool Const_0(Element& field_element) {
    for (bool element : field_element) {
        if (element) {
            return false; // �������� �������, �� ������� ����
        }
    }
    return true; // �� �������� ��������� ����
}

// ������� ��� ��������, �� � ������� ������� ������ ����� �������. (����������� ������� �� * )
bool Const_1(Element& field_element) {
    if (field_element.size() > 0 && field_element[0]) {
        for (size_t i = 1; i < field_element.size(); ++i) {
            if (field_element[i]) {
                return false; // �������� �������, �� ������� ����
            }
        }
        return true; // ������ ������� ������� �������, � �� ���� - ����
    }
    return false; // ������ ������� �� ������� ������� ��� ����� �������
}

// ������� ��� ��������� ���� �������� ����
Element Add(Element& field1, Element& field2) {
    Element result;
    for (size_t i = 0; i < size; ++i) {
        result[i] = field1[i] ^ field2[i]; // XOR ��� ��������� ���
    }
    return result;
}

// �������� �������� ���� 
Element Multiply(const Element& field1, const Element& field2) {
    Element result;
    result.fill(false); // Initialize the result to zero

    std::array<bool, (size * 2) - 1> temp;
    temp.fill(false);

    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            temp[i + j] ^= field1[i] & field2[j];
        }
    }

    // Define the modulus polynomial
    std::array<bool, (size * 2) - 1> mod_p;
    mod_p.fill(false);
    mod_p[0] = true;
    mod_p[1] = true;
    mod_p[2] = true;
    mod_p[10] = true;
    mod_p[173] = true;

    for (std::size_t i = (size * 2) - 2; i >= size; --i) {
        if (temp[i]) {
            for (std::size_t j = 0; j < size; ++j) {
                temp[i - size + j] ^= mod_p[j];
            }
        }
    }

    // Copy the result to the output array
    for (std::size_t i = 0; i < size; ++i) {
        result[i] = temp[i];
    }

    return result;
}

//ϳ�������� �� �������� ��. ���� 
Element Squere(const Element& field1) {
    Element result;
    result.fill(false);

    result = Multiply(field1, field1);

    return result;
}

//ϳ�������� �� �������
Element Power(Element base, std::size_t exponent) {
    Element result;
    result.fill(false);
    result[0] = true;

    while (exponent > 0) {
        if (exponent & 1) {
            result = Multiply(result, base);
        }
        base = Multiply(base, base);
        exponent >>= 1;
    }

    return result;
}

//���������� ����
bool Trace(const Element& field) {
    Element temp = field;
    bool trace = false;

    for (std::size_t i = 0; i < size; ++i) {

        trace ^= temp[i];
        temp = Squere(temp);
    }

    return trace;
}

// ������� ��� ����������� ������� �������� �� ��������� ��������� ������
long double Power_Element(const Element& element) {
    Element temp = element;
    std::size_t power = 0;
    while (temp[0] == false) {
        temp = Multiply(temp, temp);
        power++;
    }
    return power;
}

//��������� ������� �� ���������
/*Element Inverse(const Element& field) {
    // ���������� �������, ���� �� ������ ����������
    Element r1 = field;
    // ��������� �������, �� ����� �� ������ ����������
    Element r2;
    r2.fill(true);
    // ��������� �������
    Element temp;
    temp.fill(false);

    // �������� �������� ��� �����
    std::size_t degree1 = 0;
    std::size_t degree2 = 0;
    for (const bool& value : field) {
        if (value) {
            degree1++;
        }
    }
    degree2 = size - degree1;

    // ���������� �������� ������
    while (degree2 > 0) {
        // �������� r1
        for (std::size_t i = 0; i < size; ++i) {
            temp[i] = r1[i] ^ r2[i];
        }
        r1 = Multiply(temp, r1);

        // �������� degree1
        degree1 = 0;
        for (const bool& value : r1) {
            if (value) {
                degree1++;
            }
        }

        // ���� degree1 ���� ������ �� degree2, ��� �� �������� r1 � r2
        if (degree1 < degree2) {
            std::swap(r1, r2);
            std::swap(degree1, degree2);
        }
    }

    // ���� ������� r1 �� � ��������� ���������, �� ���������� �������� �� ����
    for (const bool& value : r1) {
        if (value) {
            throw std::runtime_error("Inverse does not exist for this element.");
        }
    }

    // ��������� ��������� �������
    return r2;
}  */
Element Inverse(const Element& field) {
    Element result = field;;
    Element temp = field;

    for (size_t i = 0; i < size - 2; ++i) {
        temp = Squere(temp);
        result = Multiply(result, temp);
    }

    result = Squere(result);

    return result;
}




int main() {

    Element A;
    Element B;
    Element C;
    std::array<size_t, size> coefArrayA;
    std::array<size_t, size> coefArrayB;
    std::array<size_t, size> coefArrayC;
    size_t indexA = 0;
    size_t indexB = 0;
    size_t indexC = 0;
    int coef;

    std::cout << "Enter coefficients for element A (empty line to stop '/n'):" << std::endl;
    while (indexA < size && std::cin >> coef) {
        coefArrayA[indexA++] = coef;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter coefficients for element B (empty line to stop '/n'):" << std::endl;
    while (indexB < size && std::cin >> coef) {
        coefArrayB[indexB++] = coef;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter coefficients for element C (empty line to stop '/n'):" << std::endl;
    while (indexB < size && std::cin >> coef) {
        coefArrayC[indexC++] = coef;
    }

    // ��������� ��. ���� 
    createCustomElement(A, coefArrayA, indexA);
    createCustomElement(B, coefArrayB, indexB);
    createCustomElement(C, coefArrayC, indexC);

    std::cout << "Element A:" << std::endl;
    printElement(A);
    std::cout << "Element B:" << std::endl;
    printElement(B);
    std::cout << "Element C:" << std::endl;
    printElement(C);

    // ������������ ��. ���� � ����������� 
    std::array<size_t, size> coefArrayResultA;
    size_t arraySizeA;
    toCoefficientArray(A, coefArrayResultA, arraySizeA);

    std::array<size_t, size> coefArrayResultB;
    size_t arraySizeB;
    toCoefficientArray(B, coefArrayResultB, arraySizeB);

    std::array<size_t, size> coefArrayResultC;
    size_t arraySizeC;
    toCoefficientArray(C, coefArrayResultC, arraySizeC);

    std::cout << "Coefficient array for A:" << std::endl;
    printCoef(coefArrayResultA, arraySizeA);
    std::cout << "Coefficient array for B:" << std::endl;
    printCoef(coefArrayResultB, arraySizeB);
    std::cout << "Coefficient array for C:" << std::endl;
    printCoef(coefArrayResultC, arraySizeC);

    //�������� ������� �� � ��. �����
    std::cout << "Const 0 ? :" << std::endl;
    std::cout << (Const_0(A) ? "True" : "False") << std::endl;

    //�������� ������� �� � ��. ��������
    std::cout << "Const 1 ? :" << std::endl;
    std::cout << (Const_1(B) ? "True" : "False") << std::endl;

    //��������� ���� �������� ���� 
    Element add;
    add = Add(A, B);
    std::cout << "A + B  :" << std::endl;
    printElement(add);
    std::array<size_t, size> coefArrayResult_add;
    size_t arraySize_add;
    toCoefficientArray(add, coefArrayResult_add, arraySize_add);

    std::cout << "Coefficient array for A + B:" << std::endl;
    printCoef(coefArrayResult_add, arraySize_add);

    //�������� �������� ���� 
    Element mul;
    mul = Multiply(A, B);
    std::cout << "A * B  :" << std::endl;
    printElement(mul);
    std::array<size_t, size> coefArrayResult_mul;
    size_t arraySize_mul;
    toCoefficientArray(mul, coefArrayResult_mul, arraySize_mul);

    std::cout << "Coefficient array for A * B:" << std::endl;
    printCoef(coefArrayResult_mul, arraySize_mul);

    //ϳ�������� �� �������� ��. ���� 
    Element squere;
    squere = Squere(A);
    std::cout << "A ^ 2  :" << std::endl;
    printElement(squere);
    std::array<size_t, size> coefArrayResult_squere;
    size_t arraySize_squere;
    toCoefficientArray(squere, coefArrayResult_squere, arraySize_squere);

    std::cout << "Coefficient array for A ^ 2 :" << std::endl;
    printCoef(coefArrayResult_squere, arraySize_squere);

    //ϳ�������� �� ������� 
    std::size_t power = 10;
    Element powe = Power(A, power);
    std::cout << "A ^ power  :" << std::endl;
    printElement(powe);
    std::array<size_t, size> coefArrayResult_powe;
    size_t arraySize_powe;
    toCoefficientArray(powe, coefArrayResult_powe, arraySize_powe);

    std::cout << "Coefficient array for A ^ power :" << std::endl;
    printCoef(coefArrayResult_powe, arraySize_powe);

    //���������� ����
    bool trace_A = Trace(A);
    std::cout << "The trace of the element A is: " << (trace_A ? "true" : "false") << std::endl;

    bool trace_B = Trace(B);
    std::cout << "The trace of the element B is: " << (trace_B ? "true" : "false") << std::endl;

    //��������� ������ �� ��������� 
    Element inverse;
    inverse = Inverse(A);
    std::cout << "A ^ (-1)  :" << std::endl;
    printElement(inverse);
    std::array<size_t, size> coefArrayResult_inverse;
    size_t arraySize_inverse;
    toCoefficientArray(inverse, coefArrayResult_inverse, arraySize_inverse);

    std::cout << "Coefficient array for A ^ (-1) :" << std::endl;
    printCoef(coefArrayResult_inverse, arraySize_inverse);


    //���������� 

    //(a + b) * c = b*c + c*a
    std::cout << "Testting : (a + b) * c = b*c + c*a " << std::endl;
    Element test1;
    test1 = Multiply(Add(A, B), C);
    std::cout << " (a + b) * c :" << std::endl;
    printElement(test1);
    std::array<size_t, size> coefArrayResult_test1;
    size_t arraySize_test1;
    toCoefficientArray(test1, coefArrayResult_test1, arraySize_test1);

    Element test2;
    Element a = Multiply(B, C);
    Element b = Multiply(C, A);
    test2 = Add(a, b);
    std::cout << " b*c + c*a :" << std::endl;
    printElement(test2);
    std::array<size_t, size> coefArrayResult_test2;
    size_t arraySize_test2;
    toCoefficientArray(test2, coefArrayResult_test2, arraySize_test2);

    std::cout << " (a + b) * c :" << std::endl;
    printCoef(coefArrayResult_test1, arraySize_test1);
    std::cout << " b*c + c*a :" << std::endl;
    printCoef(coefArrayResult_test2, arraySize_test2);

}
