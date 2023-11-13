#include <stdio.h>
// #include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>

// struct node는 연결리스트의 노드를 정의하는 구조체
// Stack의 노드로, 변수, 함수, 함수 호출, 시작, 종료 등을 표현하기 위한 정보를 담고 있다
struct node
{
    /* int type : 노드의 종류를 나타내는 멤버 변수
    1. 변수 (variable)
    2. 함수 (function)
    3. 함수 호출 (function call)
    4. 시작 (begin)
    5. 종료 (end)*/
    int type;
    char exp_data;     // 노드가 표현하는 데이터 또는 연산자
    int val;           // 노드가 표현하는 값
    int line;          // 노드가 위치한 라인(줄)
    struct node *next; // 다음 노드를 가리키는 포인터
};

// struct node 를 Node로 타입명 재정의
typedef struct node Node;

// 노드를 저장하는 스택을 표현하는 구조체
struct stack
{
    Node *top;
};

// struct stack을 Stack으로 재정의
typedef struct stack Stack;

// 연산자 스택의 노드를 표현하는 구조체 정의
struct opnode
{
    char op;
    struct opnode *next;
};

// struct opnode를 opNode로 타입명을 재정의
typedef struct opnode opNode;

// 연산자 스택을 표현하는 구조체 정의의
struct opstack
{
    opNode *top;
};

// struct opstack을 OpStack으로 타입명 재정의
typedef struct opstack OpStack;

// 후위 표기법 계산에 사용되는 스택의 노드를 표현하는 구조체를 정의
struct postfixnode
{
    int val;
    struct postfixnode *next;
};

// struct postfixnode 를 Postfixnode로 타입명을 재정의
typedef struct postfixnode Postfixnode;

// 후위 표기법 계산에 사용되는 스택을 표현하는 구조체를 정의
struct postfixstack
{
    Postfixnode *top;
};

// struct postfixstack을 PostfixStack으로 타입명을 재정의
typedef struct postfixstack PostfixStack;

void clrscr();
// 주어진 문자에 대한 값을 반환하는 역할
// Stack에서 해당 값을 찾아 반환하고, 라인 번호도 함께 가져옴
// 반환값은 변수의 값을 나타내며, 라인 번호는 함수 호출일 때 사용
int GetVal(char, int *, Stack *);

// 스택에서 마지막 함수 호출의 라인 번호를 반환
// 스택을 검사해서 마지막으로 호출된 함수를 찾고 그 함수의 라인 번호를 반환 
int GetLastFunctionCall(Stack *);

// 스택에 있는 모든 노드를 해제하고 스택의 메모리 해제
// 스택을 반환하고 반환된 스택을 NULL로 설정
Stack *FreeAll(Stack *);

// 스택에 노드를 추가하는 함수
// 새로운 노드를 동적으로 할당하고 노드에 값을 복사한 후, 스택의 맨 위에서 새로운 노드 추가
Stack *Push(Node sNode, Stack *stck)
{
    Node *newnode;

    /* 새로운 노드를 동적으로 할당
    단, 동적으로 할당한 노드가 NULL일 경우 예외 처리*/
    if ((newnode = (Node *)malloc(sizeof(Node))) == NULL)
    {
        printf("ERROR, Couldn't allocate memory...");
        return NULL;
    }
    else
    {
        // 새로운 노드에 값을 복사하고 스택에 추가
        newnode->type = sNode.type;
        newnode->val = sNode.val;
        newnode->exp_data = sNode.exp_data;
        newnode->line = sNode.line;
        newnode->next = stck->top;
        stck->top = newnode;
        return stck;
    }
}

// 연산자 스택에 연산자를 추가하는 함수
// 새로운 연산자 노드를 동적으로 할당하고 연산자를 설정한 후, 연산자 스택의 맨 위에 추가
OpStack *PushOp(char op, OpStack *opstck)
{
    opNode *newnode;
    /*연산자 노드를 동적으로 할당
    단, 할당한 노드가 NULL일 경우 예외 처리*/
    if ((newnode = (opNode *)malloc(sizeof(opNode))) == NULL)
    {
        printf("ERROR, Couldn't allocate memory...");
        return '\0';
    }
    else
    {
        // 연산자 노드에 값을 설정하고 스택에 추가
        newnode->op = op;
        newnode->next = opstck->top;
        opstck->top = newnode;
        return opstck;
    }
}

// 연산자 스택에서 연산자를 pop하는 함수
char PopOp(OpStack *opstck)
{
    opNode *temp;
    char op;
    // 스택이 비어 있다면 오류 메시지 출력
    if (opstck->top == NULL)
    {
        printf("ERROR, empty stack...");
    }
    // 스택 상단에서 연산자를 검색하고 해당 노드에 할당된 메모리를 해제한 다음 연산자를 반환
    else
    {
        op = opstck->top->op;
        temp = opstck->top;
        opstck->top = opstck->top->next;
        free(temp);
        return op;
    }
    return '\0';
}

/*postfix 스택에 푸쉬
새 노드에 메모리를 할당하고 해당 값을 설정한 후 스택 맨 위에 추가*/
PostfixStack *PushPostfix(int val, PostfixStack *poststck)
{
    Postfixnode *newnode;
    // 메모리 동적 할당한 후 그 값이 NULL일 경우 에러 메시지 출력
    if ((newnode = (Postfixnode *)malloc(sizeof(Postfixnode))) == NULL)
    {
        printf("ERROR, Couldn't allocate memory...");
        return '\0';
    }
    else
    {
        newnode->val = val;
        newnode->next = poststck->top;
        poststck->top = newnode;
        return poststck;
    }
}

// 스택에서 값을 팝하는 함수
char PopPostfix(PostfixStack *poststck)
{
    Postfixnode *temp;
    int val;
    // 스택이 비어 있으면 오류 메시지를 출력
    if (poststck->top == NULL)
    {
        printf("ERROR, empty stack...");
    }
    // 
    else
    {
        val = poststck->top->val;
        temp = poststck->top;
        // top를 제거하고 next를 가리킴
        poststck->top = poststck->top->next;
        // 메모리 해제
        free(temp);
        return val;
    }
    return '\0';
}

void Pop(Node *sNode, Stack *stck)
{
    Node *temp;

    // 스택이 비어있다면 에러 문구 출력
    if (stck->top == NULL)
    {
        printf("ERROR, empty stack...");
    }
    // 최상위 노드에서 다음으로 데이터 복사
    else
    {
        // exp_data를 최상위 노드에 복사
        sNode->exp_data = stck->top->exp_data;
        // type을 최상위 노드에 복사
        sNode->type = stck->top->type;
        // line을 최상위 노드에 복사
        sNode->line = stck->top->line;
        // val을 최상위 노드에 복사
        sNode->val = stck->top->val;
        // temp에 최상위 노드의 주소를 저장
        temp = stck->top;
        // top 포인터를 스택의 다음 노드로 이동
        stck->top = stck->top->next;
        // 메모리 해제 -> 메모리 누수 방지
        free(temp);
    }
}

int isStackEmpty(OpStack *stck)
{
    if (stck->top == 0)
        return 1;
    return 0;
}

/*void printAllStack(Stack * stck)
{
    Node tempNode;
    printf("\n------------------\n");
    printf("dumping the stack...\n");
    Pop(&tempNode,stck);
    printf("exp=%c type=%d val=%d 	line=%d\n",tempNode.exp_data,tempNode.type,tempNode.val,tempNode.line);
    while( (stck->top!=NULL) ) {
        Pop(&tempNode,stck);
        printf("exp=%c type=%d val=%d 				line=%d\n",tempNode.exp_data,tempNode.type,tempNode.val,tempNode.line);
    }
} */

int Priotry(char operator)
{
    if ((operator== '+') || (operator== '-'))
        return 1;
    else if ((operator== '/') || (operator== '*'))
        return 2;
    // 그 외 경우
    return 0;
}

// 화면을 지우는 함수
void clrscr()
{
    // clear : UNIX 및 Linux 시스템에서 화면을 지우는 명령어
    // cls : windows 운영 체제에서 터미널 화면을 지우는 명령어
    system("clear || cls");
}

int main(int argc, char **argv)
{
    // char형 문자 배열 선언
    char line[4096];
    char dummy[4096];
    char lineyedek[4096];
    char postfix[4096];

    // char형 포인터 선언
    char *firstword;

    //	int형 변수 선언
    int val1;
    int val2;
    int LastExpReturn;
    int LastFunctionReturn = -999;
    int CalingFunctionArgVal;

    // Node 구조체(tempNode) 선언
    Node tempNode;

    // OpStack * MathStack;
    // OpStack이라는 구조체의 포인터인 MathStack을 동적 할당
    OpStack *MathStack = (OpStack *)malloc(sizeof(OpStack));
    // 할당에 실패할 경우 오류 메시지 출력 후 '3' 반환
    if (MathStack == NULL)
    {
        printf("Error, Couldn't allocate memory by MathStack");
        return 3;
    }
    // 할당에 성공하면 top을 NULL로 초기화
    MathStack->top = NULL;

    FILE *filePtr;

    // PostfixStack * CalcStack;
    // PostfixStack이라는 구조체 포인터인 CalcStack을 동적으로 할당
    PostfixStack *CalcStack = (PostfixStack *)malloc(sizeof(PostfixStack));
    // 할당에 실패하면 메모리 할당 오류를 출력하고 '4' 반환
    if (CalcStack == NULL)
    {
        printf("Error, Couldn't allocate memory by CalcStack");
        return 4;
    }
    // 할당에 성공하면 top을 NULL로 초기화
    CalcStack->top = NULL;

    int resultVal;

    // Stack * STACK;
    // STack이라는 구조체 포인터인 STACK을 동적으로 할당
    Stack *STACK = (Stack *)malloc(sizeof(Stack));
    // 할당에 실패하면 메모리 할당 오류를 출력하고 '5' 반환
    if (STACK == NULL)
    {
        printf("Error, Couldn't allocate memory by STACK");
        return 5;
    }
    // 할당에 성공하면 top을 NULL로 초기화
    STACK->top = NULL;

    // int형 변수 선언 및 0으로 초기값 설정
    int curLine = 0;
    int foundMain = 0;
    int WillBreak = 0;

    // 각각의 스택의 top를 NULL로 초기화
    MathStack->top = NULL;
    CalcStack->top = NULL;
    STACK->top = NULL;
    // clrscr() 함수 호출 -> 화면을 지우는 역할
    clrscr();

    // 인수의 개수가 2가 아닐 경우 에러 문구 출력
    if (argc != 2)
    {
        printf("Incorrect arguments!\n");
        // argv[0] = 프로그램의 이름
        printf("Usage: %s <inputfile.spl>", argv[0]);
        return 1;
    }

    // 파일 열기에 실패한 경우 에러 문구 출력
    if ((filePtr = fopen(argv[1], "r")) == NULL)
    {
        perror("Error opening file by yeeun");
        return 2;
    }

    // 파일의 끝에 도달할 때까지 반복
    while (!feof(filePtr))
    {

        int k = 0;

        // 파일에서 한 줄씩 데이터를 읽어오는 함수
        // line 배열에 최대 4096자까지 읽어온다
        fgets(line, 4096, filePtr);
        // 현재 읽은 line에서 '\0'을 만날 때까지 while문 반복
        while (line[k])
        {
            // 현재 문자가 '\t'(탭)인지 확인
            if (line[k] == '\t')
            {
                // 만약 '\t'라면 그 위치의 문자를 스페이스로 대체
                line[k] = ' ';
            }
            k++;
        }
        // 현재 line을 lineyedek에 복사
        strcpy(lineyedek, line);
        // 현재 line의 번호를 증가
        curLine++;
        // tempNode의 val, exp_data, line, type 등 멤버 변수의 초기값 설정
        tempNode.val = -999;
        tempNode.exp_data = ' ';
        tempNode.line = -999;
        tempNode.type = -999;

        // strcmpi 에서 strcasecmp로 변경
        // strcasecmp -> 문자열을 대소문자 구분 없이 비교
        /* 주어진 문자열 line이 "begin" 또는 "begin\n"과 일치하는지
        아니면 "end" 또는 "end\n"과 일치하는지 확인하고, 일치하는 경우에 특정 동작을 수행*/
        if (!strcasecmp("begin\n", line) | !strcasecmp("begin", line))
        {
            if (foundMain)
            {
                // begin과 일치하는 경우 tempNode.type 값을 '4'로 설정
                tempNode.type = 4;
                // STACK에 tempNode를 Push
                STACK = Push(tempNode, STACK);
            }
        }
        else if (!strcasecmp("end\n", line) | !strcasecmp("end", line))
        {
            if (foundMain)
            {
                int sline;

                // end와 일치하는 경우
                tempNode.type = 5;
                STACK = Push(tempNode, STACK);
                
                // GetLastFunctionCall -> 스택에서 마지막으로 호출된 함수의 라인 번호를 가져오는 함수
                sline = GetLastFunctionCall(STACK);
                if (sline == 0)
                {
                    /* WE FOUND THE RESULT! */
                    printf("Output=%d", LastExpReturn);
                }
                else
                {
                    int j;
                    int foundCall = 0;
                    // 마지막으로 호출된 함수의 반환값(LastExpReture)을 LastFunctionReturn에 저장
                    LastFunctionReturn = LastExpReturn;
                    // 현재 열려있는 파일을 닫는다
                    fclose(filePtr);
                    // 파일을 다시 열어서 처음부터 시작
                    filePtr = fopen(argv[1], "r");
                    // 파일의 현재 라인을 0으로 초기화
                    curLine = 0;
                    // 파일 역방향 탐색
                    // 역방향으로 탐색하면서 sline에 저장된 라인 번호까지 이동
                    for (j = 1; j < sline; j++)
                    {
                        // 파일을 역방향으로 한줄씩 읽어온다
                        fgets(dummy, 4096, filePtr);
                        // 현재 라인 번호를 증가
                        curLine++;
                    }

                    // 스택 정리
                    // 스택을 계속해서 pop하면서 type이 3인 경우를 찾는 함수
                    while (foundCall == 0)
                    {
                        Pop(&tempNode, STACK);
                        if (tempNode.type == 3)
                        {
                            // type이 3일 경우 foundCall의 값을 1로 설정하고 루프 종료
                            foundCall = 1;
                        }
                    }
                }
            }
        }
        else
        {
            // line을 공백을 기준으로 strtok 함수를 사용하여 토큰화
            firstword = strtok(line, " ");

            // firstword를 int와 대소문자를 구분하지 않고 비교
            if (!strcasecmp("int", firstword))
            {
                if (foundMain)
                {
                    //tempNode.type을 1로 설정
                    tempNode.type = 1;
                    firstword = strtok(NULL, " ");
                    // 변수 이름의 첫 번째 문자를 tempNode.exp_data에 저장
                    tempNode.exp_data = firstword[0];

                    firstword = strtok(NULL, " ");
                    // 다음 토근이 "="이면 다시 strtok을 사용해서 변수의 초기값을 가져온다
                    if (!strcasecmp("=", firstword))
                    {
                        firstword = strtok(NULL, " ");
                    }
                    // tempNode.val에 변수의 정수 값으로 변환한 값을 저장
                    tempNode.val = atoi(firstword);
                    // tempNode.line에 0을 저장 
                    tempNode.line = 0;
                    // STACK에 tempNode를 푸쉬
                    STACK = Push(tempNode, STACK);
                }
            }
            // firstword를 function과 대소문자 구분하지 않고 비교
            // 만약 firstword와 function이 같다면 아래 블록 실행
            else if (!strcasecmp("function", firstword))
            {
                //tempNode.type 값을 '2'로 설정
                tempNode.type = 2;
                //strtok 함수를 사용하여 다음 토큰을 가져온다
                firstword = strtok(NULL, " ");
                // 함수 이름의 첫 번째 문자를 저장
                tempNode.exp_data = firstword[0];
                // tempNode.line에 curLine 저장
                tempNode.line = curLine;
                // tempNode.val에 '0' 저장
                tempNode.val = 0;
                // STACK에 tempNode를 푸쉬
                STACK = Push(tempNode, STACK);

                // 만약 함수 이름이 'main'이면 foundMain을 '1'로 설정
                if ((firstword[0] == 'm') & (firstword[1] == 'a') & (firstword[2] == 'i') & (firstword[3] == 'n'))
                    foundMain = 1;
                else
                {
                    if (foundMain)
                    {
                        firstword = strtok(NULL, " ");
                        // tempNode를 생성하고 초기값 설저
                        tempNode.type = 1;
                        tempNode.exp_data = firstword[0];
                        tempNode.val = CalingFunctionArgVal;
                        tempNode.line = 0;
                        // 생성된 노드를 STACK에 Push
                        STACK = Push(tempNode, STACK);
                    }
                }
            }
            // 다음 토큰이 '('인 경우
            else if (firstword[0] == '(')
            {

                if (foundMain)
                {
                    int i = 0;
                    int y = 0;
                    // MathStack의 top 값 초기화 
                    MathStack->top = NULL;
                    // 후위 표기법 변환 while문
                    // lineyedek 배열의 끝에 도달할 때까지 루프 반복
                    while (lineyedek[i] != '\x0')
                    {
                        // 현재 문자가 숫자인 경우
                        if (isdigit(lineyedek[i]))
                        {
                            // 해당 문자를 후위 표기법 배열(postfix)에 추가
                            postfix[y] = lineyedek[i];
                            y++;
                        }
                        else if (lineyedek[i] == ')')
                        {
                            if (!isStackEmpty(MathStack))
                            {
                                postfix[y] = PopOp(MathStack);
                                y++;
                            }
                        }
                        // 현재 문자가 (+, -, *, /)인 경우 스택에 연산자를 푸쉬하거나 팝하는 작업
                        else if ((lineyedek[i] == '+') | (lineyedek[i] == '-') | (lineyedek[i] == '*') | (lineyedek[i] == '/'))
                        {
                            // MathStack이 비어 있는지 확인
                            // 만약 비어 있다면, 현재 연산자 lineyedek[i]를 스택에 푸쉬
                            if (isStackEmpty(MathStack))
                            {
                                MathStack = PushOp(lineyedek[i], MathStack);
                            }
                            // 비어 있지 않을 경우 연산자의 우선 순위를 비교
                            else
                            {
                                // 만약 현재 연산자(lineyedek[i])의 우선 순위가 스택의 맨 위 연산자의 우선 순위보다 작거나 같다면
                                if (Priotry(lineyedek[i]) <= Priotry(MathStack->top->op))
                                {
                                    // 높은 우선순위의 연산자가 위에 있는 경우에는 스택에서 팝하여 'postfix' 배열에 추가
                                    postfix[y] = PopOp(MathStack);
                                    y++;

                                    MathStack = PushOp(lineyedek[i], MathStack);
                                }
                                else
                                {
                                    // 스택의 맨 위 연산자보다 낮은 우선 순위를 가지므로 현재 연산자를 스택에 푸쉬
                                    MathStack = PushOp(lineyedek[i], MathStack);
                                }
                            }
                        }
                        // 현재 문자가 알파벳 문자인지 확인
                        else if (isalpha(lineyedek[i]) > 0)
                        {
                            // 해당 변수 또는 함수 호출의 코드 line 번호가 저장된다
                            int codeline = 0;
                            int dummyint = 0;
                            int retVal = 0;
                            // Getval 함수를 호출하여 현재 문자(lineyedek[i])에 대한 값을 얻음
                            retVal = GetVal(lineyedek[i], &codeline, STACK);
                            // 반환값이 -1 또는 -999가 아닌 경우 실행
                            if ((retVal != -1) & (retVal != -999))
                            {
                                // 변수인 경우
                                // ASCII 값으로 변환하여 postfix 배열에 추가
                                postfix[y] = retVal + 48;
                                y++;
                            }
                            else
                            {

                                if (LastFunctionReturn == -999)
                                {
                                    // 함수 호출인 경우
                                    int j;
                                    // tempNode에 함수 호출임을 나타내는 type 설정
                                    tempNode.type = 3;
                                    // 현재 line 번호 설정
                                    tempNode.line = curLine;
                                    // STACK에 푸쉬
                                    STACK = Push(tempNode, STACK);

                                    // 함수의 인수 값(CalingFunctionArgVal)을 얻어온다
                                    CalingFunctionArgVal = GetVal(lineyedek[i + 2], &dummyint, STACK);
                                    // 파일 포인터를 닫고 다시 열어서 파일의 시작으로 돌아간다
                                    fclose(filePtr);
                                    filePtr = fopen(argv[1], "r");
                                    curLine = 0;
                                    for (j = 1; j < codeline; j++)
                                    {
                                        fgets(dummy, 4096, filePtr);
                                        curLine++;
                                    }
                                    WillBreak = 1;
                                    break;
                                }
                                else
                                {
                                    // LastFunctionReturn 값을 ASCII 값으로 변환하여 postfix 배열에 추가
                                    postfix[y] = LastFunctionReturn + 48;
                                    y++;
                                    i = i + 3;
                                    LastFunctionReturn = -999;
                                }
                            }
                        }
                        i++;
                    }
                    // WillBreak는 이전 코드에서 설정한 플래그로 0이면 계산이 필요한 상태를 나타냄
                    if (WillBreak == 0)
                    {
                        // 비어 있지 않은 동안 스택에서 연산자를 팝하여 postfix 배열에 추가 
                        while (isStackEmpty(MathStack) == 0)
                        {

                            postfix[y] = PopOp(MathStack);
                            y++;
                        }
                        // 문자열의 끝을 나타내는 NULL 문자 추가
                        postfix[y] = '\0';
                        // 반복문을 위해 i 값 초기화
                        i = 0;
                        // 연산 스택 CalcStack 초기화
                        CalcStack->top = NULL;
                        // postfix 배열의 끝에 도달할 때까지 반복
                        while (postfix[i] != '\x0')
                        {
                            // 현재 문자가 숫자인 경우, 숫자를 스택에 Push
                            if (isdigit(postfix[i]))
                                CalcStack = PushPostfix(postfix[i] - '0', CalcStack);
                            // 현재 문자가 연산자인 경우, 연산자를 스택에서 Pop하여 연산을 수행하고 결과를 다시 스택에 Push
                            else if ((postfix[i] == '+') | (postfix[i] == '-') | (postfix[i] == '*') | (postfix[i] == '/'))
                            {
                                val1 = PopPostfix(CalcStack);
                                val2 = PopPostfix(CalcStack);

                                switch (postfix[i])
                                {
                                case '+':
                                    resultVal = val2 + val1;
                                    break;
                                case '-':
                                    resultVal = val2 - val1;
                                    break;
                                case '/':
                                    resultVal = val2 / val1;
                                    break;
                                case '*':
                                    resultVal = val2 * val1;
                                    break;
                                }
                                CalcStack = PushPostfix(resultVal, CalcStack);
                            }
                            i++;
                        }
                        // 수식을 계산하고 결과를 LastExpReturn에 저장
                        LastExpReturn = CalcStack->top->val;
                    }
                    WillBreak = 0;
                }
            }
        }
    }

    // 파일 닫기
    fclose(filePtr);
    // STACK에 할당된 메모리 해제
    STACK = FreeAll(STACK);
    printf("\nPress a key to exit...");
    // getchar() 함수를 사용하여 사용자의 키 입력을 기다림
    getchar();
    // getch();
    return 0;
}

// Stack * FreeAll(Stack * stck)
// {
// Node * temp;
// Node * head;

// 	if (stck->top != NULL )
// 	{
// 		head=stck->top;
// 		do
// 		{

// 			temp=head;
// 			head=head->next;
// 			free(temp);

// 		} while (head->next!=NULL);
// 	}
// 	return '\0';
// }

// 수정한 부분
// 메모리 해제
Stack *FreeAll(Stack *stck)
{
    Node *temp;

    while (stck->top != NULL)
    {
        temp = stck->top;
        stck->top = stck->top->next;
        free(temp);
    }
    return NULL;
}

// 주어진 스택에서 마지막으로 호출된 함수의 라인 번호를 찾는 함수
int GetLastFunctionCall(Stack *stck)
{
    Node *head;

    // 스택이 비어 있는 경우
    if (stck->top == NULL)
    {
        printf("ERROR, empty stack...");
    }
    else
    {
        // head를 스택의 맨 위에 있는 노드로 설정
        head = stck->top;
        do
        {
            if (head->type == 3)
                // 해당 함수 호출의 line 번호 반환
                return head->line;
            else
                //다음 노드로 이동
                head = head->next;
        } while (head->next != NULL);
    }
    return 0;
}

// 주어진 스택에서 함수 호출에 대한 값을 찾는 함수
int GetVal(char exp_name, int *line, Stack *stck)
{
    // 구조체를 가리키는 포인터 head 선언
    Node *head;
    *line = 0;
    // 스택이 비어 있는 경우, 에러 메시지 출력
    if (stck->top == NULL)
    {
        printf("ERROR, empty stack...");
    }
    else
    {
        // head를 스택의 맨 위에 있는 노드로 설정
        head = stck->top;
        // head->next가 NULL이 될 때까지 do-while 루프 실행
        do
        {
            if (head->exp_data == exp_name)
            {
                if (head->type == 1)
                {
                    // 해당 변수의 값을 반환
                    return head->val;
                }
                else if (head->type == 2)
                {
                    // 함수 호출의 line 번호를 *line에 저장
                    *line = head->line;
                    return -1;
                }
            }
            // 일치하는 노드가 없는 경우
            else
            {
                // 다음 노드로 이동
                head = head->next;
            }
        } while (head->next != NULL);
        // 한 번 더 Check
        if (head->exp_data == exp_name)
        {

            if (head->type == 1)
            {
                return head->val;
            }
            else if (head->type == 2)
            {
                *line = head->line;
                return -1;
            }
        }
    }
    // 일치하는 값이 없는 경우 '-999' 반환
    return -999;
}