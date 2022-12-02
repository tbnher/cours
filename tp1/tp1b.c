#include <stdio.h>
#include <stdlib.h>

// Le nombre de nouvelles case qui seront ajoutées
// dès que la pile sera pleine
#define PAS_ALLOCATION 5

// Une pile de taille fixe
typedef struct {
    // un pointeur vers un tableau d'éléments
    element_t *pile;
    // l'indice du dernier élément empilé
    int sommet;
    // la taille du tableau pile
    int taille;
} pile_variable_t;

/// Initialise une pile. La pile
/// doit être vide après initialisation.
///
/// Retourne 0 en cas de succès, -1 en cas d'erreur
/// (l'allocation dynamique a échoué par exemple)
int pile_variable_initialiser(pile_variable_t *pile){
    pile->sommet=0;
    pile->taille=PAS_ALLOCATION;
    pile->pile = malloc(sizeof(element_t) * pile->taille);
    return (pile->pile) ? 0 : -1;
}

/// Retourne vrai si la pile est vide, 0 sinon
/// (on rappelle qu'en C, 0 est faux, tout le reste est vrai)
int pile_variable_est_vide(const pile_variable_t *pile){
    return pile->sommet==0;
}

/// Ajoute un élément dans la pile pointée par pile
///
/// Retourne 0 si l'élément a pu être ajouté, -1 sinon
/// (en cas de pile pleine par exemple)
int pile_variable_empiler(pile_variable_t *pile, element_t element){
    if (pile->sommet>=pile->taille){
        element_t* p = realloc(pile->pile,sizeof(element)*(pile->taille+PAS_ALLOCATION));
        if (p == null){
            return -1;
        }
        pile->pile = p;
    }
    pile->pile[pile->sommet]=element;
    return 0;
}

/// Supprime un élément de la pile et le copie
/// à l'adresse pointée par p_element.
///
/// Retourne 0 en cas de succès, -1 sinon
/// (si la pile était vide par exemple)
int pile_variable_depiler(pile_variable_t *pile, element_t *p_element)
    if (pile_variable_est_vide(pile)){
        return -1;
    }
    free(pile->pile[pile->sommet])
    realloc(sizeof(element)*--pile->taille)
    return ? 0 : -1;
/// Affiche les éléments contenus dans la pile
void pile_variable_afficher(const pile_variable_t *pile)
{
    for (int i = 0; i < pile->sommet; i++) {
        printf("%d",pile->pile[i]);
    }
}
    
/// Detruit la pile
void pile_variable_detruire(pile_variable_t *pile);
{

}






#include <assert.h>

void test_pile_variable(void) {

    pile_variable_t pile;

    pile_variable_initialiser(&pile);

    // Une pile initialisée doit être vide
    assert(pile_variable_est_vide(&pile));

    assert(pile_variable_empiler(&pile, 1) == 0);
    assert(pile_variable_empiler(&pile, 2) == 0);
    assert(pile_variable_empiler(&pile, 3) == 0);
    assert(pile_variable_empiler(&pile, 4) == 0);
    assert(pile_variable_empiler(&pile, 5) == 0);
    assert(pile_variable_empiler(&pile, 6) == 0);
    assert(pile_variable_empiler(&pile, 7) == 0);

    pile_variable_afficher(&pile);

    element_t e;
    assert(pile_variable_depiler(&pile, &e) == 0);
    assert(e == 7);
    assert(pile_variable_depiler(&pile, &e) == 0);
    assert(e == 6);
    assert(pile_variable_depiler(&pile, &e) == 0);
    assert(e == 5);
    assert(pile_variable_depiler(&pile, &e) == 0);
    assert(e == 4);
    assert(pile_variable_depiler(&pile, &e) == 0);
    assert(e == 3);
    assert(pile_variable_depiler(&pile, &e) == 0);
    assert(e == 2);
    assert(pile_variable_depiler(&pile, &e) == 0);
    assert(e == 1);

    assert(pile_variable_depiler(&pile, &e) == -1);

    assert(pile_variable_est_vide(&pile));

    pile_variable_detruire(&pile);
}