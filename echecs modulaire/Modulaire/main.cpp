#include <iostream>
using namespace std;

enum class Piece { VIDE, PION, TOUR, ROI, REINE, FOU, CAVALIER };

struct Case {
    Piece piece;
    int joueur;
};

class Plateau {
private:
    Case cases[8][8];

public:
    Plateau() {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                cases[i][j].piece = Piece::VIDE;
                cases[i][j].joueur = 0;
            }
        }

        cases[0][1].piece = Piece::CAVALIER;
        cases[0][1].joueur = 1;

        cases[0][6].piece = Piece::CAVALIER;
        cases[0][6].joueur = 1;

        cases[7][6].piece = Piece::CAVALIER;
        cases[7][6].joueur = 2;

        cases[7][1].piece = Piece::CAVALIER;
        cases[7][1].joueur = 2;

        cases[0][2].piece = Piece::FOU;
        cases[0][2].joueur = 1;

        cases[0][5].piece = Piece::FOU;
        cases[0][5].joueur = 1;

        cases[7][5].piece = Piece::FOU;
        cases[7][5].joueur = 2;

        cases[7][2].piece = Piece::FOU;
        cases[7][2].joueur = 2;

        cases[0][3].piece = Piece::REINE;
        cases[0][3].joueur = 1;

        cases[7][3].piece = Piece::REINE;
        cases[7][3].joueur = 2;

        cases[0][0].piece = Piece::TOUR;
        cases[0][0].joueur = 1;

        cases[0][7].piece = Piece::TOUR;
        cases[0][7].joueur = 1;

        cases[7][0].piece = Piece::TOUR;
        cases[7][0].joueur = 2;

        cases[7][7].piece = Piece::TOUR;
        cases[7][7].joueur = 2;

        cases[0][4].piece = Piece::ROI;
        cases[0][4].joueur = 1;

        cases[7][4].piece = Piece::ROI;
        cases[7][4].joueur = 2;

        for (int i = 0; i < 8; i++) {
            cases[1][i].piece = Piece::PION;
            cases[1][i].joueur = 1;
        }

        for (int i = 0; i < 8; i++) {
            cases[6][i].piece = Piece::PION;
            cases[6][i].joueur = 2;
        }
    }

    void afficherPlateau() {
        cout << "  a b c d e f g h" << endl;
        for (int i = 0; i < 8; ++i) {
            cout << 8 - i << " ";
            for (int j = 0; j < 8; ++j) {
                switch (cases[i][j].piece) {
                case Piece::VIDE:
                    cout << ". ";
                    break;
                case Piece::TOUR:
                    cout << "T ";
                    break;
                case Piece::ROI:
                    cout << "R ";
                    break;
                case Piece::PION:
                    cout << "P ";
                    break;
                case Piece::REINE:
                    cout << "Q ";
                    break;
                case Piece::CAVALIER:
                    cout << "C ";
                    break;
                case Piece::FOU:
                    cout << "F ";
                    break;
                default:
                    cout << "? ";
                    break;
                }
            }
            cout << endl;
        }
    }

    bool deplacerReine(int departX, int departY, int arriveeX, int arriveeY) {
        if ((departX != arriveeX && departY != arriveeY) &&
            (abs(departX - arriveeX) != abs(departY - arriveeY))) {
            return false; // Déplacement invalide pour une reine
        }

        // Vérification de présence de pièces entre la position de départ et d'arrivée
        int deltaX = (arriveeX - departX > 0) ? 1 : (arriveeX - departX < 0) ? -1 : 0;
        int deltaY = (arriveeY - departY > 0) ? 1 : (arriveeY - departY < 0) ? -1 : 0;

        int x = departX + deltaX;
        int y = departY + deltaY;

        while (x != arriveeX || y != arriveeY) {
            if (cases[x][y].piece != Piece::VIDE) {
                return false; // Déplacement invalide, pièce présente sur le chemin
            }
            x += deltaX;
            y += deltaY;
        }

        cases[arriveeX][arriveeY] = cases[departX][departY];
        cases[departX][departY].piece = Piece::VIDE;
        cases[departX][departY].joueur = 0;
        return true;
    }

    bool deplacerFou(int departX, int departY, int arriveeX, int arriveeY) {
        if (abs(departX - arriveeX) != abs(departY - arriveeY)) {
            return false; // Déplacement invalide pour un fou
        }

        // Vérification de présence de pièces entre la position de départ et d'arrivée
        int deltaX = (arriveeX - departX > 0) ? 1 : -1;
        int deltaY = (arriveeY - departY > 0) ? 1 : -1;

        int x = departX + deltaX;
        int y = departY + deltaY;

        while (x != arriveeX || y != arriveeY) {
            if (cases[x][y].piece != Piece::VIDE) {
                return false; // Déplacement invalide, pièce présente sur le chemin
            }
            x += deltaX;
            y += deltaY;
        }

        cases[arriveeX][arriveeY] = cases[departX][departY];
        cases[departX][departY].piece = Piece::VIDE;
        cases[departX][departY].joueur = 0;
        return true;
    }

    bool deplacerCavalier(int departX, int departY, int arriveeX, int arriveeY) {
        int deltaX = abs(departX - arriveeX);
        int deltaY = abs(departY - arriveeY);

        if ((deltaX == 1 && deltaY == 2) || (deltaX == 2 && deltaY == 1)) {
            cases[arriveeX][arriveeY] = cases[departX][departY];
            cases[departX][departY].piece = Piece::VIDE;
            cases[departX][departY].joueur = 0;
            return true;
        }
        return false; // Déplacement invalide pour un cavalier
    }

    bool deplacerTour(int departX, int departY, int arriveeX, int arriveeY) {
        if (cases[arriveeX][arriveeY].piece != Piece::VIDE) {
            return false; // Déplacement invalide, case occupée
        }

        if (departX != arriveeX && departY != arriveeY) {
            return false; // Déplacement invalide pour une tour
        }

        cases[arriveeX][arriveeY] = cases[departX][departY];
        cases[departX][departY].piece = Piece::VIDE;
        cases[departX][departY].joueur = 0;
        return true;
    }

    bool deplacerPion(int departX, int departY, int arriveeX, int arriveeY, int joueur) {
        int direction = (joueur == 1) ? 1 : -1;

        if (departY == arriveeY) {
            if (cases[arriveeX][arriveeY].piece != Piece::VIDE) {
                return false; // Déplacement invalide, case occupée
            }

            if ((departX + direction == arriveeX) && (cases[arriveeX][arriveeY].piece == Piece::VIDE)) {
                cases[arriveeX][arriveeY] = cases[departX][departY];
                cases[departX][departY].piece = Piece::VIDE;
                cases[departX][departY].joueur = 0;
                return true;
            }
        }
        else if ((departX + direction == arriveeX) && (abs(departY - arriveeY) == 1) &&
            (cases[arriveeX][arriveeY].piece != Piece::VIDE)) {
            cases[arriveeX][arriveeY] = cases[departX][departY];
            cases[departX][departY].piece = Piece::VIDE;
            cases[departX][departY].joueur = 0;
            return true;
        }
        return false;
    }

    bool deplacerRoi(int departX, int departY, int arriveeX, int arriveeY) {
        if (abs(departX - arriveeX) <= 1 && abs(departY - arriveeY) <= 1) {
            cases[arriveeX][arriveeY] = cases[departX][departY];
            cases[departX][departY].piece = Piece::VIDE;
            cases[departX][departY].joueur = 0;
            return true;
        }
        return false;
    }
};

int main() {
    Plateau plateau;

    while (true) {
        plateau.afficherPlateau();

        char departColonne, arriveeColonne;
        int departLigne, arriveeLigne;

        cout << "Joueur 1 (Blancs), entrez votre mouvement (ex: a2 a4): ";
        cin >> departColonne >> departLigne >> arriveeColonne >> arriveeLigne;

        int departX = 8 - departLigne;
        int departY = departColonne - 'a';
        int arriveeX = 8 - arriveeLigne;
        int arriveeY = arriveeColonne - 'a';

        if (plateau.deplacerTour(departX, departY, arriveeX, arriveeY)) {
            cout << "Mouvement valide." << endl;
        }
        else if (plateau.deplacerPion(departX, departY, arriveeX, arriveeY, 1)) {
            cout << "Mouvement valide." << endl;
        }
        else if (plateau.deplacerRoi(departX, departY, arriveeX, arriveeY)) {
            cout << "Mouvement valide." << endl;
        }
        else if (plateau.deplacerReine(departX, departY, arriveeX, arriveeY)) {
            cout << "Mouvement valide." << endl;
        }
        else if (plateau.deplacerFou(departX, departY, arriveeX, arriveeY)) {
            cout << "Mouvement valide." << endl;
        }
        else if (plateau.deplacerCavalier(departX, departY, arriveeX, arriveeY)) {
            cout << "Mouvement valide." << endl;
        }
        else {
            cout << "Mouvement invalide." << endl;
        }
    }

    return 0;
}
