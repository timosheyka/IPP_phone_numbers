/** @file
 * Moduł pierwszej częsći dużego zadania IPP
 *
 * @author Tsimafei Lukashevich
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/** Iłość cyfr */
#define SIZE 12

/**@struct node
    @var prfx_arr - konkretny numer
    @var next - wskaznik na następny numer
    @var parent - wskaznik na ojca
 */
struct node{
    char *prfx_arr;
    struct node *next;
    struct node *parent;
};
typedef struct node node;

/** @struct PhoneForward
    @var child - tablica synów każdej cyfry;
    @var prefix - struktura symboli , na którą zamieniamy prefix;
    @var parent - wskaznik na poprzednij element;
 */
struct PhoneForward{
    struct PhoneForward *child[SIZE];
    node *prefix;
    struct PhoneForward *parent;
};
typedef struct PhoneForward PhoneForward;

/** @struct PhoneNumbers
    @var numbers - wskaznik na listę numerów;
    @var arr_length - dłougość numeru
 */
struct PhoneNumbers{
    node *numbers;
    size_t arr_length;
};
typedef struct PhoneNumbers PhoneNumbers;


/** @brief Tworzy nową strukturę.
 * Tworzy nową strukturę niezawierającą żadnych przekierowań.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
PhoneForward * phfwdNew(void) {
    PhoneForward * tmp = (PhoneForward *) malloc(sizeof(PhoneForward));
    if (tmp == NULL) return NULL;

    tmp->parent = NULL;
    tmp->prefix = NULL;
    for(int i = 2; i < SIZE; i++)
        tmp->child[i] = NULL;


    tmp->child[0] = (PhoneForward *) malloc(sizeof(PhoneForward));
    PhoneForward * tmp_num = tmp->child[0];
    if(tmp_num == NULL) { free(tmp); return NULL; }
    tmp_num->parent = tmp;
    tmp_num->prefix = NULL;
    for (int i = 0; i < SIZE; i++)
        tmp_num->child[i] = NULL;

    tmp->child[1] = (PhoneForward *) malloc(sizeof(PhoneForward));
    PhoneForward * tmp_pref = tmp->child[1];
    if(tmp_pref == NULL) {
        free(tmp);
        free(tmp_num);
        return NULL;
    }
    tmp_pref->parent = tmp;
    tmp_pref->prefix = NULL;
    for (int i = 0; i < SIZE; i++)
        tmp_pref->child[i] = NULL;

    return tmp;
}
/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p pf. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] pf – wskaźnik na usuwaną strukturę.
 */
void phfwdDelete(PhoneForward *pf) {
    if (pf != NULL) {
        PhoneForward  *current = pf;
        PhoneForward *parentRoot = pf->parent;
        PhoneForward *root = pf;
        bool first = true;

        while (current != parentRoot) {
            if(current == root && !first) root->child[0] = NULL;
            bool check = false;
            first = false;
            for (int i = 0; i < SIZE; i++) {
                if (current->child[i] != NULL) {
                    current = current->child[i];
                    check = true; break;
                }
            }
            if (!check) {
                PhoneForward *parent = current->parent;
                if (parent != NULL)
                    for (int i = 0; i < SIZE; i++)
                        if (parent->child[i] == current) { parent->child[i] = NULL; break; }
                node *head = current->prefix;
                if(head!=NULL) {
                    if(!parentRoot) {
                        if(!root->child[0]) {
                            while (head->next != NULL) {
                                node *tmp = head->next;
                                free(head->prfx_arr);
                                free(head);
                                head = tmp;
                            }
                        }
                    }
                    free(head->prfx_arr);
                    free(head);
                }
                free(current);
                current = parent;
            }
        }
    }
}

/** @brief Sprawdza , czy ciąg symboli jest numerem.
 * @param[in] num - numer , który będziemy sprawdzali.
 * @return boolean(true or false)
 */
bool check_num(char const *num) {
    int i = 0;
    if (num == NULL) return false;
    if (strcmp(num, "") == 0) return false;
    while (true) {
        if(!isdigit(num[i]) && num[i] != '*' && num[i] != '#' && num[i] != '\0')
            return false;
        if(num[i] == '\0') break;
        i++;
    }
    return true;
}

/** @brief Zwraca liczbe w systemie 11 cyfrowym
 *
 * @param[in] num - dostarczony numer
 * @return liczbę odpowiadającą pewnemu znaku
 */
int get_digit(char num) {
    switch (num) {
        case '*' : return 10;
        case '#' : return 11;
        default : return num - '0';
    }
}


/** @brief Tworzy nowego syna
 *
 * @param[in] parent - wskaznik na ojca.
 * @param[in] index - index.
 * @return
 */
PhoneForward *newChild(PhoneForward *parent, int index) {
    PhoneForward * newChild = (PhoneForward *) malloc(sizeof(PhoneForward));
    if (newChild == NULL) return NULL;

    newChild->parent = parent;
    parent->child[index] = newChild;
    newChild->prefix = NULL;

    for (int i = 0; i < SIZE; i++)
        newChild->child[i] = NULL;

    return newChild;
}
/** @brief Porównuje ciąg numerów
 *
 * @param[in] num1 - numer 1
 * @param[in] num2 - numer 2
 * @return czy jest num1 > num2 w leksykograficznym porównaniu
 */

int strcmp_extended(const char *num1, const char *num2) {
    size_t len1 = strlen(num1);
    size_t len2 = strlen(num2);
    size_t min_len = len1 > len2 ? len2 : len1;

    for(size_t i = 0; i < min_len; i++)
        if(num1[i] > num2[i]) return 1;
        else if(num2[i] > num1[i]) return -1;

    if(len1 == len2) return 0;
    else return len1 > len2 ? 1 : -1;
}


/** @brief Dodaje konkretne przekirowanie
 *
 * @param[in] pf – wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] num – wskaźnik na napis reprezentujący prefiks numerów
 *                     przekierowywanych;
 * @param[in] nums_or_pref - sprawdza , czy możemy przekirowywać numer
 * @param[in] root -  wskaznik na strukturę prefiksów
 * @return - true , jak uda się dodać przekirowanie , false - jak nie
 */
bool AddPrefix(PhoneForward *pf, char const *num, bool nums_or_pref, node **root) {
    size_t pref_length = strlen(num) + 1;
    node *head = pf->prefix;
    if (nums_or_pref && head) {
        while (head->next) { if (strcmp_extended(num, head->next->prfx_arr) < 1) break; head = head->next; }

        node *next = head->next;
        head->next = malloc(sizeof(node));
        if (!head->next) return false;
        head->next->prfx_arr = malloc(sizeof(char) * (strlen(num) + 1));
        if(head->next->prfx_arr == NULL) return false;
        strcpy(head->next->prfx_arr, num);
        head->next->next = next;
        head->next->parent = head;
        if (!head->next->prfx_arr) { free(head->next); return false; }
        *root = head->next;
        return true;
    } else {
        if (head != NULL) { free(head->prfx_arr); free(head); }
        head = (node *) malloc(sizeof(node));
        if (head == NULL) return false;
        head->prfx_arr = (char *) malloc(sizeof(char) * (size_t) pref_length);
        if (head->prfx_arr == NULL) {root = NULL; return false; }
        strcpy(head->prfx_arr, num);
        head->next = NULL;
        pf->prefix = head;
        head->parent = NULL;
        *root = head;
        return true;
    }
}

/** przekierowuje działanie na drzewo prefiksów lub drzewo numerów
 *
 * @param[in] pf – wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] num1   – wskaźnik na napis reprezentujący prefiks numerów
 *                     przekierowywanych;
 * @param[in] num2   – wskaźnik na napis reprezentujący prefiks numerów,
 *                     na które jest wykonywane przekierowanie.
 * @param[in] num_or_pref - sprawdza , czy możemy przekirowywać numer
 * @param[in] root -  wskaznik na strukturę prefiksów
 * @return true , jak udało się dodać
 */

bool phfwdAdd_divider(PhoneForward *pf, char const *num1, char const *num2, bool num_or_pref, node **root) {
    if (num1 == NULL || num2 == NULL) return false;
    if (check_num(num1) && check_num(num2)) {
        if (strcmp_extended(num1, num2) == 0) return false;
        PhoneForward *head = pf;
        size_t length = strlen(num1);
        for (size_t i = 0; i < length; i++) {
            if (head->child[get_digit(num1[i])] != NULL)
                head = head->child[get_digit(num1[i])];
            else {
                head = newChild(head, get_digit(num1[i]));
                if (head == NULL) return false;
            }
        }
        if (!AddPrefix(head, num2, num_or_pref, root)) return false;
        return true;
    } else return false;
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if(pf == NULL) return false;
    node *numbers = NULL;
    node *prefix = NULL;
    bool nums = phfwdAdd_divider(pf->child[0], num1, num2, false, &numbers);
    if (nums == false) { free(numbers); return false; }
    bool pref = phfwdAdd_divider(pf->child[1], num2, num1, true, &prefix);
    if(pref == true) { numbers->next = prefix; return true; }
    else return false;
}

/** @brief usuwanie konkretnego prefiksu
 *
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] head  -  wskaznik na strukturę prefiksów
 */
void prfxDelete(PhoneForward *pf, node *head){
    node *prefix = head->next;
    char *num = head->prfx_arr;
    if (prefix->parent != NULL) {
        prefix->parent->next = prefix->next;
        if (prefix->next) prefix->next->parent = prefix->parent;
        free(prefix->prfx_arr);
        free(prefix);
    }
    else {
        PhoneForward *tmp = pf->child[1];
        for(size_t i = 0; i < strlen(num); i++)
            tmp = tmp->child[get_digit(num[i])];
        tmp->prefix = prefix->next;
        free(prefix->prfx_arr);
        free(prefix);
        if (tmp->prefix) tmp->prefix->parent = NULL;
    }
}

/** Pomocnicza funkcja do usuwania prefiksów
 *
 * @param[in] pf – wskaźnik na strukturę przechowującą przekierowania
 *             numerów;
 * @param[in] tmp - pomocniczy wskaznik
 */
void prfxDeleteHelp(PhoneForward *pf,PhoneForward *tmp){
    if(tmp == NULL) return;
    if(tmp->prefix){ prfxDelete(pf,tmp->prefix); tmp->prefix->next = NULL; }
    for(int i = 0; i < SIZE; i++)
        prfxDeleteHelp(pf,tmp->child[i] );
}
/** @brief Usuwa przekierowania.
 * Usuwa wszystkie przekierowania, w których parametr @p num jest prefiksem
 * parametru @p num1 użytego przy dodawaniu. Jeśli nie ma takich przekierowań
 * lub napis nie reprezentuje numeru, nic nie robi.
 * @param[in,out] pf – wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] num    – wskaźnik na napis reprezentujący prefiks numerów.
 */

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf == NULL) return;
    if (!check_num(num)) return;
    PhoneForward * tmp = pf->child[0];
    size_t length = strlen(num);

    for (size_t i = 0; i < length; i++)
        if(tmp->child[get_digit(num[i])])
            tmp = tmp->child[get_digit(num[i])];
        else return;

    prfxDeleteHelp(pf, tmp);
    phfwdDelete(tmp);
    tmp = NULL;
}

/** @brief realizacja przekirowania numeru
 *
 * @param[in] position - pozycja z której zaczynamy przekirowanie
 * @param[in] prefix - wskaźnik na napis reprezentujący prefix
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return przekirowany numer
 */

char * Forward(size_t position, char * prefix, const char * num) {
    size_t newnumlength = strlen(prefix) + strlen(num) - position;
    char * newnum = (char *) malloc(sizeof(char) * (newnumlength + 1));
    if (newnum == NULL) return NULL;

    for (size_t i = 0; i < newnumlength; i++)
        if(i < strlen(prefix))
            newnum[i] = prefix[i];
        else
            newnum[i] = num[i + position - strlen(prefix)];
    newnum[newnumlength] = '\0';

    return newnum;
}

/** @brief Wyznacza przekierowanie numeru.
 * Wyznacza przekierowanie podanego numeru. Szuka najdłuższego pasującego
 * prefiksu. Wynikiem jest ciąg zawierający co najwyżej jeden numer. Jeśli dany
 * numer nie został przekierowany, to wynikiem jest ciąg zawierający ten numer.
 * Jeśli podany napis nie reprezentuje numeru, wynikiem jest pusty ciąg.
 * Alokuje strukturę @p PhoneNumbers, która musi być zwolniona za pomocą
 * funkcji @ref phnumDelete.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL) return NULL;

    PhoneNumbers *pnum = (PhoneNumbers *) malloc(sizeof(PhoneNumbers));
    if(pnum == NULL) return NULL;
    PhoneForward  *tmp = (PhoneForward *) pf->child[0];

    if (!check_num(num)) {
        pnum->numbers = NULL;
        return pnum;
    } else {
        pnum->numbers =  malloc(sizeof(node));
        if (pnum->numbers == NULL) { free(pnum); return NULL; }
    }

    pnum->numbers->next = NULL;
    pnum->numbers->parent = NULL;
    pnum->numbers->prfx_arr = NULL;

    char *newnum = NULL;
    size_t i = 0;

    for (; i < strlen(num); i++) {
        if (tmp->child[get_digit(num[i])])
            tmp = tmp->child[get_digit(num[i])];
        else
            break;
    }
    for (; ; i--) {
        if (tmp == NULL) {
            newnum = malloc(sizeof(char) * (strlen(num)+1));
            if(newnum == NULL) return NULL;
            strcpy(newnum, num);
            break;
        } else {
            if (tmp->prefix) {
                newnum = Forward(i, tmp->prefix->prfx_arr, num);
                break;
            } else
                tmp = tmp->parent;
        }
    }

    pnum->numbers->prfx_arr = newnum;
    pnum->arr_length = 1;
    return pnum;
}

/** @brief wylicza długość drzewa prefisków
 *
 * @param[in] prefix - wskaznik na strukturę prefiksów
 * @return długość drzewa prefiksów
 */
int prefix_size(node *prefix){
    int res = 0;
    while(prefix != NULL){
        prefix = prefix->next;
        res++;
    }
    return res;
}

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p pnum. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] pnum – wskaźnik na usuwaną strukturę.
 */
void phnumDelete(PhoneNumbers *pnum) {
    if (pnum != NULL) {
        if(pnum->numbers == NULL) { free(pnum); return; }
        node * head = pnum->numbers;
        while (head != NULL) {
            node * tmp = head;
            head = head->next;
            if (tmp->prfx_arr != NULL)
                free(tmp->prfx_arr);
            free(tmp);
        }
        free(pnum);
    }
}

/** @brief Udostępnia numer.
 * Udostępnia wskaźnik na napis reprezentujący numer. Napisy są indeksowane
 * kolejno od zera.
 * @param[in] pnum – wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 * @param[in] idx  – indeks numeru telefonu.
 * @return Wskaźnik na napis reprezentujący numer telefonu. Wartość NULL, jeśli
 *         wskaźnik @p pnum ma wartość NULL lub indeks ma za dużą wartość.
 */
char const * phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if (pnum == NULL)
        return NULL;
    if (idx >= pnum->arr_length)
        return NULL;
    node * head = pnum->numbers;
    for (size_t i = 0; i < idx; i++)
        head = head->next;
    return head->prfx_arr;
}

/** @brief Wyznacza prefix
 *
 * @param[in] pf - – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] pnum - wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 * @param[in] num - numer , na którym operujemy
 * @param[in] prefix_list - wskaźnik na strukturę przechowującą listę prefiksów
 * @param[in] position - pozycja od której zaczynamy dziłać
 * @param[in] isGet - zmienna boolowa
 * @return
 */
bool getPrefix(const PhoneForward *pf, PhoneNumbers ** pnum, const char *num, node * prefix_list, size_t position, bool isGet){
    PhoneNumbers *tmp = * pnum;
    node *numbers = tmp->numbers;
    size_t len = prefix_size(prefix_list);
    tmp->arr_length += len;
    node *tmpnumbers;

    bool flag = true;
    for(size_t i = 0; i < len; i++) {
        flag = true;
        node *pref_tmp = malloc(sizeof(node));
        if (pref_tmp == NULL) return false;
        tmpnumbers = numbers;
        pref_tmp->prfx_arr = Forward(position + 1, prefix_list->prfx_arr, num);
        if(isGet){
            PhoneNumbers *pnum_tmp = phfwdGet(pf, pref_tmp->prfx_arr);
            if(strcmp_extended(phnumGet(pnum_tmp, 0), num) != 0){
                phnumDelete(pnum_tmp);
                free(pref_tmp->prfx_arr);
                tmp->arr_length--;
                free(pref_tmp);
                prefix_list = prefix_list->next;
                continue;
            }
            phnumDelete(pnum_tmp);
        }
        if(tmpnumbers == NULL){
            pref_tmp->next = NULL;
            pref_tmp->parent = NULL;
            tmpnumbers = pref_tmp;
            tmp->numbers = tmpnumbers;
            flag = false;
        }
        else if(tmpnumbers->next != NULL) {
            while (strcmp_extended(tmpnumbers->prfx_arr, pref_tmp->prfx_arr) < 1) {
                if(strcmp_extended(tmpnumbers->prfx_arr, pref_tmp->prfx_arr) == 0){
                    tmp->arr_length--;
                    break;
                }
                if(tmpnumbers->next == NULL){
                    pref_tmp->next = NULL;
                    pref_tmp->parent = tmpnumbers;
                    tmpnumbers->next = pref_tmp;
                    flag = false;
                    break;
                }
                tmpnumbers = tmpnumbers->next;
            }
            if(flag) {
                pref_tmp->next = tmpnumbers;
                pref_tmp->parent = tmpnumbers->parent;
                if (pref_tmp->parent)pref_tmp->parent->next = pref_tmp;
                else tmp->numbers = pref_tmp;
                tmpnumbers->parent = pref_tmp;
            }
        }
        else if (tmpnumbers->next == NULL){
            if (strcmp_extended(tmpnumbers->prfx_arr, pref_tmp->prfx_arr) == -1) {
                tmpnumbers->next = pref_tmp;
                pref_tmp->next = NULL;
                pref_tmp->parent = tmpnumbers;
            }
            else if (strcmp_extended(tmpnumbers->prfx_arr, pref_tmp->prfx_arr) == 1) {
                pref_tmp->next = tmpnumbers;
                tmpnumbers->parent = pref_tmp;
                pref_tmp->parent = NULL;
                tmp->numbers = numbers = pref_tmp;
            }
            else tmp->arr_length--;
        }
        prefix_list = prefix_list->next;
        numbers = tmp->numbers;

    }
    *pnum = tmp;
    return true;
}

/** @brief Wyznacza przekierowania na dany numer.
 * Wyznacza następujący ciąg numerów: jeśli istnieje numer @p x, taki że wynik
 * wywołania @p phfwdGet z numerem @p x zawiera numer @p num, to numer @p x
 * należy do wyniku wywołania @ref phfwdReverse z numerem @p num. Dodatkowo ciąg
 * wynikowy zawsze zawiera też numer @p num. Wynikowe numery są posortowane
 * leksykograficznie i nie mogą się powtarzać. Jeśli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciąg. Alokuje strukturę
 * @p PhoneNumbers, która musi być zwolniona za pomocą funkcji @ref phnumDelete.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
 PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num) {
    if(pf == NULL) return NULL;

    PhoneForward *tmp = pf->child[1];
    PhoneNumbers *pnum = malloc(sizeof(PhoneNumbers));
    if(pnum == NULL) return NULL;

    if(!check_num(num)){
        pnum->arr_length = 0;
        pnum->numbers = NULL;
        return pnum;
    }
    if(!num || !strlen(num) ){
        pnum->arr_length = 0;
        pnum->numbers = NULL;
        return pnum;
    }

    pnum->numbers = malloc(sizeof(node));
    if(pnum->numbers == NULL) { free(pnum); return NULL; }

    pnum->numbers->prfx_arr = malloc(sizeof(char) * (strlen(num) + 1));
    if(pnum->numbers->prfx_arr == NULL) return NULL;
    strcpy(pnum->numbers->prfx_arr, num);
    pnum->numbers->parent = NULL;
    pnum->numbers->next = NULL;
    pnum->arr_length = 1;

    size_t numlen = strlen(num);
    for(size_t i = 0; i < numlen; i++){
        if(tmp->child[get_digit(num[i])])
            tmp = tmp->child[get_digit(num[i])];
        else
            break;
        if(tmp->prefix)
            if(!getPrefix(pf, &pnum, num, tmp->prefix, i, false))
                return NULL;
    }
    return pnum;
}


/**@brief Wyznacza liste numerów
 * wyznacza posortowaną leksykograficznie listę wszystkich takich numerów
 * telefonów i tylko takich numerów telefonów x, że phfwdGet(x) = num.
 * @param[in] pf - wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na listę numerów.
 */

PhoneNumbers *phfwdGetReverse(PhoneForward const *pf, char const *num) {
    if (pf == NULL)return NULL;
    PhoneForward *tmp = pf->child[1];
    PhoneNumbers *pnum = malloc(sizeof(PhoneNumbers));
    if (pnum == NULL) return NULL;

    if (!check_num(num)) {
        pnum->arr_length = 0;
        pnum->numbers = NULL;
        return pnum;
    }
    if (!num || !strlen(num)) {
        pnum->arr_length = 0;
        pnum->numbers = NULL;
        return pnum;
    }
    PhoneNumbers *pnum_2 = phfwdGet(pf, num);
    if (strcmp_extended(phnumGet(pnum_2, 0), num) == 0) {
        pnum->numbers = malloc(sizeof(node));
        if (!pnum->numbers)return NULL;

        pnum->numbers->prfx_arr = malloc(sizeof(char) * (strlen(num) + 1));
        if (pnum->numbers->prfx_arr == NULL) return NULL;
        strcpy(pnum->numbers->prfx_arr, num);

        pnum->numbers->parent = NULL;
        pnum->numbers->next = NULL;
        pnum->arr_length = 0;
    }
    else {
        pnum->numbers = NULL;
        pnum->arr_length = 0;
    }
    if (pnum_2 != NULL) phnumDelete(pnum_2);

    size_t numlen = strlen(num);
    for(size_t i = 0; i < numlen; i++){
        if(tmp->child[get_digit(num[i])])
            tmp = tmp->child[get_digit(num[i])];
        else
            break;
        if(tmp->prefix)
            if(!getPrefix(pf, &pnum, num, tmp->prefix, i, true))
                return NULL;
    }

    pnum->arr_length = 0;
    node* pnum_length = pnum->numbers;

    while (pnum_length != NULL) {
        pnum_length = pnum_length->next;
        pnum->arr_length++;
    }
    return pnum;
}