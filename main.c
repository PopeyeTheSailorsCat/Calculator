#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <crtdbg.h>
#include <string.h>
#include <ctype.h>

#pragma warning(disable:4996)
#define pi 3.14159265358979323846
#define e 2.71828182845904523536
#define eps 1e-7
typedef enum{
  Token_operator,
  Token_number
} Tokenid;

typedef struct {
  Tokenid id;
  char operator;
  double Num;
} Token;

typedef struct untypelist list;

struct untypelist{
  Token T;
  list *next;
};

typedef struct untypelist stack; //
typedef struct untypelist queue;

//works with list
stack* stack1 = NULL;
stack* stack2 = NULL;
_Bool minorError = 0;
_Bool fatalError = 0;
void ErrorHapened(int errorNumber) { //Обработка всех ошибок со всех мест программы
  if (minorError == 1) {
    return;
  }
  minorError = 1;
  switch (errorNumber){
  case -1:
    printf("ERROR: not enough memory");
    break;
  case 0://если при чтении аргументов с консоли что то пошло не так
    printf("ERROR: problem with arguments\n");
    break;
  case 1: //Везде
    printf(" == ERROR: Not enough memory\n");
    break;
  case 2: //В сканере
    printf(" == ERROR: Unrecognized character\n");
    break;
  case 3: // в определителе команд
    printf(" == ERROR: Unrecogized command\n");
    break;
  case 4://любые ошибки со скобками в парсере
    printf(" == ERROR: brackets\n");
    break;
  case 5://В evale
    printf(" == ERROR: not enough numbers for operations\n");
    break;
  case 6://В evale
    printf(" == ERROR: divide to zero\n");
    break;
  case 7://В evale
    printf(" == ERROR: ln argument below zero\n");
    break;
  case 8://В evale
    printf(" == ERROR: argument below zero under root\n");
    break;
  case 9:
    printf(" == ERROR: unexpected end\n");
    break;
  case 10:
    printf(" == ERROR: Function range violation\n");
    break;
  case 11:
    printf(" == ERROR: Raising a negative number to a non-integer power\n");
      break;
  default:
    break;
  }
  if (fatalError != 0) {
    exit(1);
  }
  return;
}

void Push(stack **head, Token newelem) {
  stack *tmp = (stack*)_malloc_dbg(sizeof(stack), _CLIENT_BLOCK, NULL, 0);
  if (tmp == NULL) {
    ErrorHapened(1);
    return;
  }
  tmp->T = newelem;
  tmp->next = (*head);
  (*head) = tmp;
  return;
}

int Put(queue **head, Token newelem) {
  queue *tmp = (queue*)_malloc_dbg(sizeof(queue), _CLIENT_BLOCK, NULL, 0);
  if (tmp == NULL) {
    ErrorHapened(1);
    return 1;
  }
  tmp->T = newelem;
  tmp->next = (*head);
  (*head) = tmp;
  return 0;
}


queue* Pop(stack **head) {
  stack* prev = NULL;
  if (*head == NULL) {
    return NULL;
  }
  prev = (*head);
  (*head) = (*head)->next;
  return prev;
}


queue* GetLastButOne(queue* head) {
  if (head == NULL) {
    exit(-2);
  }
  if (head->next == NULL) {
    return NULL;
  }
  while (head->next->next) {
    head = head->next;
  }
  return head;
}

void PrintToken(Token T) {
  if (T.id == Token_operator) {
    printf("op:%c ", T.operator);
  }
  else if (T.id == Token_number) {
    printf("N:%f ", T.Num);
  }
  else
    printf("Unknow token");
}


void PrintLinkedList(const queue *head) {
  while (head) {
    PrintToken(head->T);
    head = head->next;
  }
  printf("\n");
}

void deleteList(list **head) {
  list* prev = NULL;
  while ((*head)->next) {
    prev = (*head);
    (*head) = (*head)->next;
    _free_dbg(prev, _CLIENT_BLOCK);
  }
  _free_dbg(*head, _CLIENT_BLOCK);
}

void deleteQueue(queue **head) {
  queue* prev = NULL;
  if (*head == NULL) {
    return;
  }
  while ((*head)->next) {
    prev = (*head);
    (*head) = (*head)->next;
    _free_dbg(prev, _CLIENT_BLOCK);
  }
  _free_dbg(*head, _CLIENT_BLOCK);
  *head = NULL;
  return;
}
void deleteStack(stack **head) {
  stack* prev = NULL;
  while ((*head)->next) {
    prev = (*head);
    (*head) = (*head)->next;
    _free_dbg(prev, _CLIENT_BLOCK);
  }
  _free_dbg(*head, _CLIENT_BLOCK);
}
int ChechSymbol(char s, char second) {//Проверяем не закончилась ли строка, пока мы читаем
  if (s == '\0' && second =='y') {
    ErrorHapened(2);
    return 0;
  }else if(s=='\0'){
    ErrorHapened(2);
    return 0;
  }
  return 1;
}

queue *PopBack(queue **head) {
  queue* lastbn = NULL;
  queue* Tok = NULL;
  //Получили NUL
  if (!head) {
    return NULL;
  }
  //Список пуст
  if (!(*head)) {
    return NULL;
  }
  lastbn = GetLastButOne(*head);
  //Если в списке один элемент
  if (lastbn== NULL) {
    Tok = *head;
    *head = NULL;
  }
  else {
    Tok = (&(*lastbn->next));
    lastbn->next = NULL;
  }
  return Tok;
}
char FunctRecogn(char*s, char **here) {//Посимвольный разбор функций //@ - sin// s-sqrt// c-cos// r - ctg //t-tg
  char str[10];                         // f-floor// o-ceil // j arcsin// k arccos// i arctg
  switch (*s){
  case 'a':
    s++;
    if (!ChechSymbol(*s, 'y')) {
      return 'q';
    }
    str[0] = *s;
    s++;
    if (!ChechSymbol(*s, 'n')) {
      return 'q';
    }
    str[1] = *s;
    s++;
    if (!ChechSymbol(*s, 'n')) {
      return 'q';
    }
    str[2] = *s;
    s++;
    if (!ChechSymbol(*s, 'n')) {
      return 'q';
    }
    str[3] = *s;
    //s++;
    if (str[0] == 'r' && str[1] == 'c' && str[2] == 't' && str[3] == 'g') {
      s++;
      *here = s;
      return 'i';
    }
    s++;
    if (!ChechSymbol(*s, 'n')) {
      return 'q';
    }
    str[4] = *s;
    if (str[0] == 'r' && str[1] == 'c' && str[2] == 's' && str[3] == 'i' && str[4] == 'n') {
      s++;
      *here = s;
      return 'j';
    }
    else if (str[0] == 'r' && str[1] == 'c' && str[2] == 'c' && str[3] == 'o' && str[4] == 's') {
      s++;
      *here = s;
      return 'k';
    }
    else {
      ErrorHapened(2);
      return 'q';
      //error
    }
    break;
  case 'f':
    s++;
    if (!ChechSymbol(*s, 'y')) {
      return 'q';
    }
    str[0] = *s;
    s++;
    if (!ChechSymbol(*s, 'n')) {
      return 'q';
    }
    str[1] = *s;
    s++;
    if (!ChechSymbol(*s, 'n')) {
      return 'q';
    }
    str[2] = *s;
    s++;
    if (!ChechSymbol(*s, 'n')) {
      return 'q';
    }
    str[3] = *s;
    if (str[0] == 'l' && str[1] == 'o' && str[2] == 'o' && str[3] == 'r') {
      s++;
      *here = s;
      return 'f';
    }
    else {
      //error
      ErrorHapened(2);
    }
    break;
  case 'l':
    s++;
    if (*s == 'n') {
      s++;
      *here = s;
      return 'l';
    }
    else {
      ErrorHapened(2);
      //er
    }
    break;
  case 't':
    s++;
    if (*s == 'g') {
      s++;
      *here = s;
      return 't';
    }
    else {
      ErrorHapened(2);
      //error
    }
    break;
  case 'c':
    s++;
    if (!ChechSymbol(*s, 'y')) {
      return 'q';
    }
    //char str[2];
    str[0] = *s;
    s++;
    if (!ChechSymbol(*s, 'n')) {
      return 'q';
    }
    str[1] = *s;
    if (str[0] == 'o' && str[1] == 's') {
      s++;
      *here = s;
      return 'c';
    }
    else if (str[0] == 't' && str[1] == 'g'){
      s++;
      *here = s;
      return 'r';
    }
    else if (str[0] == 'e' &&str[1] == 'i') {
      s++;
      if (!ChechSymbol(*s, 'n')) {
        return 'q';
      }
      if (*s == 'l') {
        s++;
        *here = s;
        return 'o';
      }
      else {
        ErrorHapened(2);
      }
    }
    else{//error
        ErrorHapened(2);
    }
    break;
  case 's':
    s++;
    if (!ChechSymbol(*s, 'y')) {
      return 'q';
    }
    switch (*s) {
    case 'i':
      s++;
      if (!ChechSymbol(*s, 'n')) {
        return 'q';
      }
      if (*s == 'n') {

        s++;
        *here = s;
        return '@';
      }
      else {
        ErrorHapened(2);
        //error
      }
      break;
    case 'q': //опасное место проверка?
      s++;
      if (!ChechSymbol(*s, 'n')) {
        return 'q';
      }
      //char str[2];
      str[0] = *s;
      s++;
      if (!ChechSymbol(*s, 'n')) {
        return 'q';
      }
      str[1] = *s;
      if (str[0] == 'r' &&str[1] == 't') {
        s++;
        *here = s;
        return 's';
      }
      else {
        ErrorHapened(2);
        return 'q';
      }
      // error;
      break;
    default:
      ErrorHapened(2);
      return 'q';
    }
  }
  return 'q';
}//посимвольный разбор функций
void PrintBuffer(char *str, int count) {//Проверочная функция
  for (int i = 0; i < count; i++) {
    printf("%c", str[i]);
  }
  return;
}
int Scan(queue **q, char *s, int len) {//Читаем полученную строку
  queue elem;
  queue *Tok = &elem;
//  queue *prev = NULL;
  _Bool textBefore= 0; //для строк комментариев
  char *here=NULL;
  int count = 0;
  while (minorError !=1 && count<len) { 
    here = s; 
    switch (*s) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
    case '\v':
    case '\b':
    case '\f':
          s++;
      count++;
      continue;
    case '-':
      Tok->T.id = Token_operator;
      Tok->T.operator='-';
      s++;
      textBefore = 1;
      break;
    case '+':
      Tok->T.id = Token_operator;
      Tok->T.operator='+';
      s++;
      textBefore = 1;
      break;
 //   case '.':
 //     ErrorHapened(2);
 //     break;
    case '*':
      Tok->T.id = Token_operator;
      Tok->T.operator='*';
      s++;
      textBefore = 1;
      break;
    case '^':
      Tok->T.id = Token_operator;
      Tok->T.operator='^';
      s++;
      textBefore = 1;
      break;
    case '/':
      //проверка на комментарий
      s++;
      if (*s == '/') {
        if (textBefore != 1) {
          minorError = 1;//позволит просто вывести строку
          printf("\n");
        }
        else {
          ErrorHapened(3);
        }
        //break;
      }
      s--;
      Tok->T.id = Token_operator;
      Tok->T.operator='/';
      s++;
      textBefore = 1;
      break;
    case '(':
      Tok->T.id = Token_operator;
      Tok->T.operator='(';
      s++;
      textBefore = 1;
      break;
    case ')':
      Tok->T.id = Token_operator;
      Tok->T.operator=')';
      s++;
      textBefore = 1;
      break;
    case 'p':
      s++;
      count++;
      if (*s == 'i') {
        Tok->T.id = Token_number;
        Tok->T.Num = pi;
      }
      else {
        //ошиб
        ErrorHapened(2);
      }
      s++;
      textBefore = 1;
      break;
    case 's':
    case 'c':
    case 't':
    case 'l':
    case 'f':
    //case 'o':
    case 'a':
      Tok->T.id = Token_operator;
      Tok->T.operator=FunctRecogn(s, &here);
      count += here - s-1;
      s = here;
      textBefore = 1;
      break;
   /* case 'E':
      prev = Pop(q);
      if (prev && prev->T.id == Token_number) {

      }*/
    case 'E':
      //prev = Pop(q);
      s--;
      if (*s > 47 && *s < 58) {
        s++;
        //PrintToken((*q)->next->T);

        s++;
        count++;
        if (*s == '+' || *s == '-' || ((*s > 47 && *s < 58))) {
          if (*s > 47 && *s < 58) {
            if (*q && (*q)->next && ((*q)->next)->T.id == Token_operator && (((*q)->next)->T.operator == 'x' || ((*q)->next)->T.operator == 'v')) {
              //PrintToken(((*q)->next)->T);
              ErrorHapened(2);
              break;
            }
            //Put(q, prev->T);
            //_free_dbg(prev, _CLIENT_BLOCK);
            Tok->T.id = Token_operator;
            Tok->T.operator = 'w'; //сверхумножение
            Put(q, Tok->T);
            Tok->T.id = Token_number;
            Tok->T.Num = 10;
            Put(q, Tok->T);
            Tok->T.id = Token_operator;
            Tok->T.operator = 'x';
            textBefore = 1;
            count--;
            break;
          }
          if (*q && (*q)->next && ((*q)->next)->T.id == Token_operator && (((*q)->next)->T.operator == 'x' || ((*q)->next)->T.operator == 'v')) {
            ErrorHapened(2);
            break;
          }
          char sym;
          if (*s == '+') {
            sym = 'x';
          }
          else {
            sym = 'v';
          }
          s++;
          //count++;
          if (*s > 47 && *s < 58) {
            //Put(q, prev->T);
            //_free_dbg(prev, _CLIENT_BLOCK);
            Tok->T.id = Token_operator;
            Tok->T.operator = 'w'; //сверхумножение
            Put(q, Tok->T);
            Tok->T.id = Token_number;
            Tok->T.Num = 10;
            Put(q, Tok->T);
            Tok->T.id = Token_operator;
            Tok->T.operator = sym;
            textBefore = 1;
            break;
          }
          else {
            ErrorHapened(2);
            //_free_dbg(prev, _CLIENT_BLOCK);
            break;
          }

        }
        else {
          ErrorHapened(2);
          //_free_dbg(prev, _CLIENT_BLOCK);
          break;
        }
      }
      else {
       // if (prev) {
       //   Put(q, prev->T);
       //   _free_dbg(prev, _CLIENT_BLOCK);
       // }
      }
      ErrorHapened(2);
      break;

    case 'e'://проверка на научную запись
      //s++;
      //prev = Pop(q);
      s--;
      if (*s > 47 && *s < 58) {

        s++;
        count++;
        s++;
        if (*s == '+' || *s == '-' || ((*s > 47 && *s < 58))) {
          if (*s > 47 && *s < 58) {
            if (*q && (*q)->next && ((*q)->next)->T.id == Token_operator && (((*q)->next)->T.operator == 'x' || ((*q)->next)->T.operator == 'v')) {
              ErrorHapened(2);
              break;
            }
            //Put(q, prev->T);
            //_free_dbg(prev, _CLIENT_BLOCK);
            Tok->T.id = Token_operator;
            Tok->T.operator = 'w'; //сверхумножение
            Put(q, Tok->T);
            Tok->T.id = Token_number;
            Tok->T.Num = 10;
            Put(q, Tok->T);

            Tok->T.id = Token_operator;
            Tok->T.operator = 'x';
            textBefore = 1;
            count--;
            break;
         }
          if (*q && (*q)->next && ((*q)->next)->T.id == Token_operator && (((*q)->next)->T.operator == 'x' || ((*q)->next)->T.operator == 'v')) {
            ErrorHapened(2);
            break;
          }
          char sym ;
          if (*s == '+') {
            sym = 'x';
          }
          else {
            sym = 'v';
          }
          s++;
          //count++;
          if (*s > 47 && *s < 58) {
            //Put(q, prev->T);
            //_free_dbg(prev, _CLIENT_BLOCK);
            Tok->T.id = Token_operator;
            Tok->T.operator = 'w'; //сверхумножение
            Put(q, Tok->T);
            Tok->T.id = Token_number;
            Tok->T.Num = 10;
            Put(q, Tok->T);
            Tok->T.id = Token_operator;
            Tok->T.operator = sym;
            textBefore = 1;
            break;
          }
          else {
            ErrorHapened(2);
            //_free_dbg(prev, _CLIENT_BLOCK);
            break;
          }

        }
        else {
          ErrorHapened(2);
          //_free_dbg(prev, _CLIENT_BLOCK);
          break;
        }
      }
      else {
        //if (prev) {
        //  Put(q, prev->T);
        //  _free_dbg(prev, _CLIENT_BLOCK);
       // }
      }
      s++;

      Tok->T.id = Token_number;
      Tok->T.Num = e;
      s++;
      textBefore = 1;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      Tok->T.id = Token_number;
      Tok->T.Num = 0;
      while (*s>47 && *s<58) {
        Tok->T.Num = Tok->T.Num * 10 + *s - '0';
        s++;
        count++;
      }
      if (*s == '.') {
        double denum = 1;
        s++;
        count++;
        if (*s <= 47 || *s >= 58) {
          ErrorHapened(2);
        }
        while (*s>47 && *s<58) {
          Tok->T.Num += (*s - '0') /( denum*= 10);
          s++;
          count++;
        }
        //count--;
      }
      count--;
      textBefore = 1;
      break;
    default:
     // if (!spaceHappened) {
        ErrorHapened(2);
        deleteQueue(q);
     // }
      break;
      //break 2;
      //Tok.id = Token_number;
      //return 0;
    }

    if (minorError != 1) { 
      if (Put(q, Tok->T) == 1) {

        //ErrorHapened(1);
        //printf("poimt");
        deleteQueue(q);
      }
    }
    else {
      deleteQueue(q);
    }

  //if (Put(&q, T) != 0) {
    count++;
  }
  //_free_dbg(Tok, _CLIENT_BLOCK);
  return 1;
}

double Eval(queue *q) {  //Выполение прочитанных операций
  queue *st= NULL;  //queue = стэк
  queue* Tok= NULL;
  queue* First = NULL;
  queue* Second=NULL;
  double result;
  Tok = (Pop(&q));
  while (Tok && minorError==0) {
    if (Tok->T.id == Token_number) {
      Push(&st,Tok->T);
    }
    else { //блок функций
      First = Pop(&st);
      Second = First;
      if (First == NULL || First->T.id == Token_operator) {
        ErrorHapened(5);
        if (First != NULL) {
          _free_dbg(First, _CLIENT_BLOCK); 
        }

        break;
      }
      switch (Tok->T.operator){
      case 'm':
          First->T.Num = -First->T.Num;
        //}
        break;
      case '@'://sin
        First->T.Num = sin(First->T.Num);
        break;
      case 's': //sqrt
                //ERROR <0
        if (First->T.Num < 0) {
          ErrorHapened(8);
        }
        else {
          First->T.Num = sqrt(First->T.Num);
        }
        break;
      case 'c': //cos
        First->T.Num = cos(First->T.Num);
        break;
      case 'r': //ctg
        if (fabs(sin(First->T.Num)) <= eps) {
          ErrorHapened(10);
        }
        else {
          First->T.Num = cos(First->T.Num) / sin(First->T.Num);
        }
        break;
      case 'l':
        //ERROR if <0
        if (First->T.Num< 1e-250){
          ErrorHapened(7);
        }
        else {
          First->T.Num = log(First->T.Num);
        }
        break;
      case 't': //tg
        if (fabs(cos(First->T.Num)) <= eps) {
          ErrorHapened(10);
        }
        else {
          First->T.Num = sin(First->T.Num) / cos(First->T.Num);
        }
        break;
      case 'f': //floor
        First->T.Num = floor(First->T.Num);
        break;
      case 'o': //ceil
        First->T.Num = ceil(First->T.Num);
        break;
      case 'j': //arcsin
        if (fabs(First->T.Num) > 1) {
          ErrorHapened(10);
        }
        else {
          First->T.Num = asin(First->T.Num);
        }
        break;
      case 'k': //arccos
        if (fabs(First->T.Num) > 1) {
          ErrorHapened(10);
        }
        else {
          First->T.Num = acos(First->T.Num);
        }
        break;
      case 'i': //arctg
       // if (fabs(First->T.Num) >= (pi / 2)) {
       //   ErrorHapened(10);
       // }
       // else {
          First->T.Num = atan(First->T.Num);
       // }
        break;
      default://меняем местами, чтобы работало не с унарными функциями
        Second = First;//Pop(&st);
        First = Pop(&st);
      }
      if (First == NULL || First->T.id==Token_operator) {//Second заведомо не null, тк уже проверяли
        ErrorHapened(5);
        if (First != NULL) {
          _free_dbg(First, _CLIENT_BLOCK);
          _free_dbg(Second, _CLIENT_BLOCK);
        }
        else {
          _free_dbg(Second, _CLIENT_BLOCK);
        }
        break;
      }
      
      switch (Tok->T.operator){
      case '+':
        First->T.Num += Second->T.Num;
        break;
      case '-': 
        if (First && First->T.id==Token_number) {
          First->T.Num -= Second->T.Num;
        }
       // else {
        //  First = Second;
        //  First->T.Num = -First->T.Num; //ayyyy lmao
       // }
        break;
      case '/'://деление на 0
        if (fabs(Second->T.Num) <= eps) {
          ErrorHapened(6);
        }
        else {
          First->T.Num /= Second->T.Num;
        }
        break;
      case '*':
        First->T.Num *= Second->T.Num;
        break;
      case '^':
        //ERROR if  под корнем у нас 0 когда степень отрицательная
        if (Second->T.Num <0 && fabs(First->T.Num) <= eps){
          ErrorHapened(6);
        }
        else if((First->T.Num)<0 && (fmod(Second->T.Num,1) !=0)){
          ErrorHapened(11);
        }
        else {
          First->T.Num = pow(First->T.Num, Second->T.Num);
        }
        break;
      case 'x'://положительная научная
        if (fmod(Second->T.Num,1)!=0) {
          ErrorHapened(2);
          break;
        }
        else {
          First->T.Num = pow(First->T.Num, Second->T.Num);
          break;
        }
        break;
      case 'v': //отрицательная научная
        if (fmod(Second->T.Num,1)!=0) {
          ErrorHapened(2);
          break;
        }
        else {
          First->T.Num = pow(First->T.Num, -(Second->T.Num));
          break;
        }
      case 'w':
        First->T.Num *= Second->T.Num;
        break;
        
      }
      Push(&st, First->T); //результат обратно в стек
      if (First != Second) {//проверяем, чтобы не чистить дважды одно и то же
        _free_dbg(First, _CLIENT_BLOCK);
        _free_dbg(Second, _CLIENT_BLOCK);
      }
      else {
        _free_dbg(First, _CLIENT_BLOCK);
      }
    }
    _free_dbg(Tok, _CLIENT_BLOCK);
    Tok = Pop(&q);
  }
  //_free_dbg(Tok, _CLIENT_BLOCK);
  if (minorError == 1) {//если где то в цикле произошла ошибка, чиститим все что использовали
    deleteQueue(&q);
    deleteQueue(&st);
    if (Tok) {
      _free_dbg(Tok, _CLIENT_BLOCK);
    }
    return 1;
  }
  if (minorError != 1) {
    First = Pop(&st);
    if (st != NULL) {
      //deleteQueue(&q);
      deleteQueue(&st);
      ErrorHapened(9);
      _free_dbg(First, _CLIENT_BLOCK);
        return 1;
    }
    result = First->T.Num;
    _free_dbg(First, _CLIENT_BLOCK);
    return result;
  }
  return 0;
}
int GetPrior(char op) {//Приоритеты операций
  switch (op){
  case '(':
    return 0;
  case ')':
    return 1;
  case '+':
  case '-':
  case 'p':
    return 2;
  case '*':
  case '/':
  case '%':
    return 3;
  case 'm':
  
    return 4;
  case '^':
  case 's':
    return 5;
  case '@':
  case 'c':
  case 'r':
  case 't':
  case 'l':
  case 'j':
  case 'k':
  case 'i':
  case 'w':
    return 6;
  case 'o':
  case 'f':
  case 'v':
  case 'x':
  //case 'w':
    return 7;
  }
  return -1;
}
int CheckAssoc(char op1, char op2) {
  int p1 = GetPrior(op1);
  int p2 = GetPrior(op2);
  if (op2 == '^' || op2 =='s') {
    return p1 > p2;
  }
  return p1 >= p2;
}
void DropOperator(char op) {
  while (stack2!= NULL && CheckAssoc(stack2->T.operator,op)) {
    queue *forUse;
    forUse = Pop(&stack2);
    Push(&stack1, forUse->T);
    _free_dbg(forUse, _CLIENT_BLOCK);
  }
}
int IsUnar(char operat) {
  switch (operat) {
  case 'm':
  case '@':
  case 's':
  case 'c':
  case 'r':
  case 'l':
  case 't':
  case 'f':
  case 'o':
  case 'j':
  case 'k':
  case 'i':
    return 1;
    break;
  case '-':
    return 2;
    break;
  default:
    return 0;
  }

}
int Parser(queue *q, queue **Qres) {
  queue *Tok= NULL;
  //Tok = PopBack(&q);
//  int isOpenBracket = 0;
  enum {
    statePrefix, stateSuffix, stateDone,stateEnd
  } state = statePrefix;
  while (state != stateEnd && minorError == 0) {
    if (state <= stateSuffix) {
      Tok = PopBack(&q);
      if (Tok == NULL) {
        if (state == stateSuffix){
          state = stateDone;
        }
        else {
          ErrorHapened(9);
          break;
          //state = stateDone;
        }
      }
    }
    switch (state) {
    case statePrefix:
      if (Tok->T.id == Token_number) {
        Push(&stack1, Tok->T);
        state = stateSuffix;
      }
      else if(Tok->T.id == Token_operator && ((Tok->T.operator=='(') || IsUnar(Tok->T.operator))){
        if (IsUnar(Tok->T.operator)==2) {
          Tok->T.operator='m';
        }

        Push(&stack2, Tok->T);
      }
      else {
        ErrorHapened(5);
        state = stateDone;
      }
      break;
    case stateSuffix:
      if (Tok->T.id == Token_number) {
        ErrorHapened(9);
        break;
      }
      if (Tok->T.operator != '(') {
        DropOperator(Tok->T.operator);
        //state = statePrefix;
      }
      //state = statePrefix;
      if (Tok->T.operator!=')'){
        Push(&stack2, Tok->T);
        state = statePrefix;
        break;
      }
      else {
        _free_dbg(Tok, _CLIENT_BLOCK);
        Tok = Pop(&stack2);
        if (!Tok || Tok->T.operator !='(') {
          ErrorHapened(4);
        }
        break;
        //state = statePrefix;
      }
     //tate = statePrefix;

      break;

    case stateDone:
      if (minorError == 0) {
        DropOperator(')');
        if (Tok) {
          _free_dbg(Tok, _CLIENT_BLOCK);
        }
        if (stack2 != NULL) {
          ErrorHapened(4);
          deleteQueue(&q);
          break;
        }
      }
      deleteQueue(&q);
        *Qres = NULL;
        while (stack1 != NULL) {
          queue *forUse;
          forUse = Pop(&stack1);
          Push(Qres, forUse->T);
          _free_dbg(forUse, _CLIENT_BLOCK);
        }
       
        state = stateEnd;
        break;
      }
    _free_dbg(Tok, _CLIENT_BLOCK);
    }
    
  if (minorError == 1) {
    if (q) {
      deleteQueue(&q);
    }
    if (stack2) {
      deleteQueue(&stack2);
    }
    if (stack1) {
      deleteQueue(&stack1);
    }
  }
  return 0;
 }
    
int main(int argc, char *argv[]) {
  FILE *fp=NULL;
  if (argc == 2) {
    //char what[100];
    //strcpy(what, argv[1]);
    fp = freopen(argv[1], "r", stdin);
    if (fp == NULL) {
      ErrorHapened(0);
      exit(1);
    }
  }
  else if (argc>2) {
    ErrorHapened(0);
    exit(1);
  }
  char* stri=NULL;
  //int Memory=10; ////////////////////////////////////////////////
  int symbol=1;
  int coun = 0;
  _Bool ex = 0;
  double result;
  queue* q=NULL;
  stri = _malloc_dbg(1*sizeof(char), _NORMAL_BLOCK, NULL, 0);
  if (stri == NULL) {
    ErrorHapened(-1);
    printf("\n");
    if (fp) {
      fclose(fp);
    }
    exit(-1);
  }
  while ((symbol = getchar()) != EOF || ex == 0) {
    if (symbol == EOF) {
      ex = 1;
    }
    if (symbol == '\n' || ex == 1) {//заходим в строку
      //printf("%s", stri);
      if (stri) {
        PrintBuffer(stri, coun);
        Scan(&q, stri, coun);
      }
      if (q == NULL && minorError == 0) {
        if (ex == 0) {
          printf("\n"); 
        }

        minorError = 1;
      }
      //PrintLinkedList(q);
      if (minorError != 1) {
        Parser(q, &q);
      }
      if (minorError != 1) {
        //PrintLinkedList(q);
        result = Eval(q);
        if (minorError != 1) {
          printf(" == %g\n", result);
        }
      }
      q = NULL;
      coun = 0;
      _free_dbg(stri, _NORMAL_BLOCK);
     // _CrtDumpMemoryLeaks();
      stri = _malloc_dbg(1*sizeof(char), _NORMAL_BLOCK, NULL, 0);
      //stri[0] = '\0';
      minorError = 0;//просмотреть тут ли это должно быть, могут быть ошибки
    }
    else {
      if (stri) {
        stri[coun] = (char)symbol;
        coun++;
      }
      stri = realloc(stri, coun + 1);
      
    //  if (coun> Memory) {///////////////////////
   //     _free_dbg(stri, _NORMAL_BLOCK);/////////////////////
   //     stri = NULL;///////////////////////
    //  }
      if (!stri) {   //ERROR if dont give more memory
        while ((symbol = getchar()) != EOF && symbol != '\n') {
          coun = 0;
        }
        coun = 0;
        ErrorHapened(-1);
        if (symbol == EOF) {
          _free_dbg(stri, _NORMAL_BLOCK);
          if (fp) {
            fclose(fp);
          }
          printf("\n");
          exit(0);
        }
        printf("\n");
        _free_dbg(stri, _NORMAL_BLOCK);
        stri = _malloc_dbg(1*sizeof(char), _NORMAL_BLOCK, NULL, 0);
        minorError = 0;

      }
        
    }
    if (ex == 1) {
      break;
    }
  }
  _free_dbg(stri, _NORMAL_BLOCK);
  if (fp) {
    fclose(fp);
  }
  //_CrtDumpMemoryLeaks();
  return 0;
}
