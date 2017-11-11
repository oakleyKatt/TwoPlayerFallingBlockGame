import math, random

def centerMatrix(matrix):
    fLeft = 5
    fRight = 0
    fTop = 5
    fBottom = 0
    sLeft = 0
    sRight = 0
    sTop = 0 
    sBottom = 0

    for row in range(0,5):
        for col in range(0,5):
            if matrix[row][col] == 1:
                if row < fTop:
                    fTop = row
                if row > fBottom:
                    fBottom = row
                if col < fLeft:
                    fLeft = col
                if col > fRight:
                    fRight = col
    
    sLeft = fLeft
    sRight = 4 - fRight
    sTop = fTop
    sBottom = 4 - fBottom


    if(sRight - sLeft > 0):
        moveH = int(math.ceil((sRight - sLeft) / 2.00))
    else:
        moveH = int(math.floor((sRight - sLeft) / 2.00)) + 1
   
    if(sBottom - sTop > 0):
        moveV = int(math.ceil((sBottom - sTop) / 2.00))
    else:
        moveV = int(math.floor((sBottom - sTop) / 2.00))
    
    return shiftV(shiftH(matrix, moveH), moveV)


def shiftH(matrix, moveH):
    myMatrix = [[0 for x in range(5)] for y in range(5)]

    if moveH > 0:
        for row in range (0, 5):
            for col in range (moveH, 5):
                myMatrix[row][col] = matrix[row][col - moveH]
    elif moveH < 0:
        for row in range (0, 5):
            for col in range (0, (5 - abs(moveH))):
                myMatrix[row][col] = matrix[row][col + abs(moveH)]
    else:
        return matrix

    return myMatrix


def shiftV(matrix, moveV):
    myMatrix = [[0 for x in range(5)] for y in range(5)]

    if moveV > 0:
        for row in range (moveV, 5):
            for col in range (0, 5):
                myMatrix[row][col] = matrix[row - moveV][col]
    elif moveV < 0:
        for row in range (0,(5 - abs(moveV))):
            for col in range (0, 5):
                myMatrix[row][col] = matrix[row + abs(moveV)][col]
    else:
        return matrix

    return myMatrix

def rotateMatrix(matrix):
    mat1 = [[0 for x in range(5)] for y in range(5)]
        
    for i in range (0, 5):
        mat1[0][4 - i] = matrix[i][0]
        mat1[1][4 - i] = matrix[i][1]
        mat1[2][4 - i] = matrix[i][2]
        mat1[3][4 - i] = matrix[i][3]
        mat1[4][4 - i] = matrix[i][4]
    
    return mat1 
    

def matrixToTemplate(matrix):
    newMatrix = ['.....', '.....', '.....', '.....', '.....']
        
    for row in range (0, 5):
        string = '';
        for col in range (0, 5):
            if matrix[row][col] == 1:
                string += '0';
            else:
                string += '.';
        newMatrix[row] = string;
    return newMatrix;


def makeTemplate(matrix):
    template = [[0 for x in range(5)] for y in range(4)]
   
    template[0] = matrixToTemplate(matrix)
    mat1 = rotateMatrix(matrix) 
    template[1] = matrixToTemplate(mat1)
    mat2 = rotateMatrix(mat1) 
    template[2] = matrixToTemplate(mat2)
    mat3 = rotateMatrix(mat2) 
    template[3] = matrixToTemplate(mat3)


def printMatrix(matrix):
    for row in range (0, 5):
        for col in range (0, 5):
            print matrix[row][col],
        print


def readMatrixFromFile():
    with open('file.txt') as f:
        array = [[int(x) for x in line.split()] for line in f]

    return array


def writeMatrixToFile():
    f = open('file.txt', 'w')

    num = random.randint(0,4)

    if (num == 0):
        f.write('0 1 0 0 0 \n')
        f.write('0 1 0 0 0 \n')
        f.write('0 1 0 0 0 \n')
        f.write('0 1 0 0 0 \n')
        f.write('0 1 0 0 0 \n')
    elif(num == 1):
        f.write('0 0 0 0 0 \n')
        f.write('0 1 1 0 0 \n')
        f.write('0 1 1 0 0 \n')
        f.write('0 0 1 1 0 \n')
        f.write('0 0 1 1 0 \n')
    elif(num == 2):
        f.write('0 0 0 0 0 \n')
        f.write('0 0 0 1 0 \n')
        f.write('0 1 1 1 1 \n')
        f.write('0 0 0 1 0 \n')
        f.write('0 0 0 0 0 \n')
    elif(num == 3):
        f.write('0 0 0 0 0 \n')
        f.write('0 1 1 0 0 \n')
        f.write('0 0 1 0 0 \n')
        f.write('1 1 1 0 0 \n')
        f.write('0 0 0 0 0 \n')
    else:
        f.write('0 0 0 0 0 \n')
        f.write('0 1 0 0 0 \n')
        f.write('1 1 1 0 0 \n')
        f.write('0 1 0 0 0 \n')
        f.write('1 0 1 0 0 \n')

    f.close()


def main():
   # matrix = [[0,0,0,0,0],
   #           [0,0,0,0,0],
   #           [1,0,0,0,0],
   #           [1,0,0,0,0],
   #           [1,1,0,0,0]]
   
    matrix = readMatrixFromFile() 

    printMatrix(matrix)
    
    matrix = centerMatrix(matrix)
    print

    printMatrix(matrix)
   
    print
    makeTemplate(matrix)

    writeMatrixToFile()


if __name__=='__main__':
    main()

