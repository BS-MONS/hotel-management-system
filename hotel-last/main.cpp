#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <map>
#ifdef _WIN32
#include <conio.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// Forward declarations (file persistence)
void sauvegarderChambres(const vector<class Chambre>& chambres);
void sauvegarderServices(const vector<class ServiceHotel>& services);

// ================== COULEURS ==================
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";

// ================== PERSONNE (BASE) ==================
class Personne {
public:
    virtual void afficher() const = 0;
    virtual string getRole() const = 0;
    virtual ~Personne() {}
};

// ================== POLYMORPHISME ==================
void presenterUtilisateur(const Personne& p) {
    cout << MAGENTA << "\n=== IDENTIFICATION (Polymorphisme) ===" << RESET << endl;
    cout << "Role : " << CYAN << p.getRole() << RESET << endl;
    p.afficher();
    cout << MAGENTA << "======================================\n" << RESET;
}

// ================== UTILITAIRE ==================
void nettoyerCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string lireLigne(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin >> ws, s);
    return s;
}

string lireDateValide(const string& prompt);

void pause(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void loadingAnimation(const string& msg) {
    cout << CYAN << msg << RESET;
    for (int i = 0; i < 3; i++) {
        cout << ".";
        cout.flush();
        pause(400);
    }
    cout << "\n";
}

// ================== DESIGN ==================
int getTerminalWidth();
std::string centerTextWithBorder(const std::string& text, int width);
void header() {
    clearScreen();
    int termWidth = getTerminalWidth();
    string border(termWidth, '=');
    cout << BLUE << border << RESET << endl;
    string headerText = "The Town  |  SYSTEME DE RESERVATION";
    cout << BLUE << centerTextWithBorder(headerText, termWidth) << RESET << endl;
    cout << BLUE << border << RESET << endl << endl;
}

int getTerminalWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width = 80;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return width;
#else
    return 80;
#endif
}

string centerTextWithBorder(const string& text, int width) {
    int pad = max(0, width - (int)text.length() - 2); // 2 for borders
    int left = pad / 2;
    int right = pad - left;
    return "|" + string(left, ' ') + text + string(right, ' ') + "|";
}

void afficherInfoHotel() {
    const string filename = "hotel_info.txt";

    ifstream in(filename);
    if (!in) {
        ofstream out(filename);
        out << "The Town Hotel\n";
        out << "Adresse: (a renseigner)\n";
        out << "Telephone: (a renseigner)\n";
        out << "Email: (a renseigner)\n";
        out << "Check-in: 14:00 | Check-out: 12:00\n";
        out.close();
        in.open(filename);
    }

    int termWidth = getTerminalWidth();
    cout << CYAN << string(termWidth, '-') << RESET << endl;
    string title = "--- INFOS HOTEL ---";
    cout << CYAN << centerTextWithBorder(title, termWidth) << RESET << endl;
    cout << CYAN << string(termWidth, '-') << RESET << endl;

    string name, address, phone, email, checkin;
    getline(in, name);
    getline(in, address);
    getline(in, phone);
    getline(in, email);
    getline(in, checkin);

    int colWidth = 22;
    string border = string(colWidth * 2 + 3, '-');
    vector<string> tableLines;
    tableLines.push_back(border);
    tableLines.push_back("|" + string(colWidth, ' ') + "Champ" + "|" + string(colWidth, ' ') + "Valeur" + "|");
    tableLines.push_back(border);
    {
        stringstream ss;
        ss << "|" << left << setw(colWidth) << "Nom de l'hotel" << "|" << left << setw(colWidth) << name << "|";
        tableLines.push_back(ss.str());
    }
    {
        stringstream ss;
        ss << "|" << left << setw(colWidth) << "Adresse" << "|" << left << setw(colWidth) << (address.length() > 9 ? address.substr(9) : address) << "|";
        tableLines.push_back(ss.str());
    }
    {
        stringstream ss;
        ss << "|" << left << setw(colWidth) << "Telephone" << "|" << left << setw(colWidth) << (phone.length() > 11 ? phone.substr(11) : phone) << "|";
        tableLines.push_back(ss.str());
    }
    {
        stringstream ss;
        ss << "|" << left << setw(colWidth) << "Email" << "|" << left << setw(colWidth) << (email.length() > 7 ? email.substr(7) : email) << "|";
        tableLines.push_back(ss.str());
    }
    {
        stringstream ss;
        ss << "|" << left << setw(colWidth) << "Check-in/out" << "|" << left << setw(colWidth) << (checkin.length() > 10 ? checkin.substr(10) : checkin) << "|";
        tableLines.push_back(ss.str());
    }
    tableLines.push_back(border);

    for (const auto& line : tableLines) {
        cout << centerTextWithBorder(line, termWidth) << endl;
    }
}

int menuPrincipal() {
    vector<string> options = {
        "Nouvelle reservation",
        "Voir chambres libres",
        "Infos hotel",
        "Signaler un probleme",
        "Acces Admin",
        "Quitter"
    };

    int selection = 0;
    while (true) {
        header();
        int termWidth = getTerminalWidth();
        string border(termWidth, '=');
        cout << MAGENTA << border << RESET << endl;
        cout << centerTextWithBorder("=== MENU PRINCIPAL ===", termWidth) << endl;
        cout << MAGENTA << border << RESET << endl;
        for (size_t i = 0; i < options.size(); ++i) {
            string line = (i == selection) ? ("-> " + options[i] + " <-") : options[i];
            string color = (i == selection) ? GREEN : RESET;
            cout << color << centerTextWithBorder(line, termWidth) << RESET << endl;
        }
        cout << CYAN << border << RESET << endl;

#ifdef _WIN32
        int key = _getch();
        if (key == 224) { // Fleches
            key = _getch();
            if (key == 72 && selection > 0) selection--;
            if (key == 80 && selection < options.size() - 1) selection++;
        } else if (key == 13) { // Entree
            return (selection == (int)options.size() - 1) ? 0 : selection + 1;
        }
#else
        int c;
        cout << "Votre choix : ";
        cin >> c;
        nettoyerCin();
        return c;
#endif
    }
}

// ================== CHAMBRE ==================
class Chambre {
public:
    int numero;
    string type;
    double prix;

    Chambre(int n, string t, double p) : numero(n), type(t), prix(p) {}
};

// ================== SERVICE ==================
class ServiceHotel {
public:
    string nom;
    double prix;

    ServiceHotel(string n, double p) : nom(n), prix(p) {}
};

// ================== CLIENT ==================
class Client : public Personne {
public:
    int id;
    string nom;
    int age;
    string telephone;
    string email;
    int nombreReservations;

    Client(int i, string n, int a, string tel, string e = "")
        : id(i), nom(n), age(a), telephone(tel), email(e), nombreReservations(0) {}

    string getRole() const override { return "Client"; }

    void afficher() const override {
        cout << "Client: " << nom << " (ID: " << id << ", Age: " << age << ", Tel: " << telephone << ")\n";
    }
};

// ================== RESERVATION ==================
class Reservation {
public:
    int id;
    int clientId;
    int numeroChambre;
    string dateDebut;
    string dateFin;
    int nuits;
    double total;
    string modePaiement;
    string nomClient;
    int ageClient;
    string telephoneClient;
    vector<ServiceHotel> services;
    double prixChambre;
    bool estActive;

    Reservation(int i, int cid, int num, string d1, string d2, int n, string nom, int age, string tel, double pc)
        : id(i), clientId(cid), numeroChambre(num), dateDebut(d1), dateFin(d2),
          nuits(n), total(0), nomClient(nom), ageClient(age), telephoneClient(tel), prixChambre(pc), estActive(true) {}
};

// ================== VALIDATEUR ==================
class BookingValidator {
public:
    struct DateParts {
        int y;
        int m;
        int d;
    };

    static bool estBissextile(int y) {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    static int joursDansMois(int y, int m) {
        static const int jours[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (m < 1 || m > 12) return 0;
        if (m == 2) return estBissextile(y) ? 29 : 28;
        return jours[m - 1];
    }

    static bool estEntier(const string& s) {
        if (s.empty()) return false;
        for (char c : s) {
            if (c < '0' || c > '9') return false;
        }
        return true;
    }

    static bool parseDateFlexible(const string& input, DateParts& out) {
        string date = input;
        if (date.empty()) return false;
        for (char& c : date) {
            if (c == '/') c = '-';
        }

        size_t first = date.find('-');
        size_t second = (first == string::npos) ? string::npos : date.find('-', first + 1);
        if (first == string::npos || second == string::npos) return false;

        string yStr = date.substr(0, first);
        string mStr = date.substr(first + 1, second - first - 1);
        string dStr = date.substr(second + 1);

        if (!estEntier(yStr) || !estEntier(mStr) || !estEntier(dStr)) return false;

        int y = stoi(yStr);
        if (yStr.size() == 2) y = 2000 + y;
        if (yStr.size() != 2 && yStr.size() != 4) return false;

        int m = stoi(mStr);
        int d = stoi(dStr);
        if (y < 1900 || y > 2200) return false;
        if (m < 1 || m > 12) return false;
        int maxJours = joursDansMois(y, m);
        if (d < 1 || d > maxJours) return false;

        out = {y, m, d};
        return true;
    }

    static bool parseISODate(const string& iso, DateParts& out) {
        if (iso.size() != 10 || iso[4] != '-' || iso[7] != '-') return false;
        string yStr = iso.substr(0, 4);
        string mStr = iso.substr(5, 2);
        string dStr = iso.substr(8, 2);
        if (!estEntier(yStr) || !estEntier(mStr) || !estEntier(dStr)) return false;
        int y = stoi(yStr);
        int m = stoi(mStr);
        int d = stoi(dStr);
        if (y < 1900 || y > 2200) return false;
        if (m < 1 || m > 12) return false;
        int maxJours = joursDansMois(y, m);
        if (d < 1 || d > maxJours) return false;
        out = {y, m, d};
        return true;
    }

    static int comparerDates(const DateParts& a, const DateParts& b) {
        if (a.y != b.y) return (a.y < b.y) ? -1 : 1;
        if (a.m != b.m) return (a.m < b.m) ? -1 : 1;
        if (a.d != b.d) return (a.d < b.d) ? -1 : 1;
        return 0;
    }

    static bool verifierDates(const string& d1, const string& d2) {
        DateParts a, b;
        if (!parseISODate(d1, a) || !parseISODate(d2, b)) return false;
        return comparerDates(a, b) < 0;
    }

    static string formaterDate(const string& input) {
        DateParts dp;
        if (!parseDateFlexible(input, dp)) return "";
        ostringstream oss;
        oss << setw(4) << setfill('0') << dp.y << "-";
        oss << setw(2) << setfill('0') << dp.m << "-";
        oss << setw(2) << setfill('0') << dp.d;
        return oss.str();
    }

    static int calculerNuits(const string& d1, const string& d2) {
        DateParts a, b;
        if (!parseISODate(d1, a) || !parseISODate(d2, b)) return -1;
        struct tm tm1 = {0}, tm2 = {0};

        tm1.tm_isdst = -1;
        tm2.tm_isdst = -1;
        tm1.tm_year = a.y - 1900;
        tm1.tm_mon  = a.m - 1;
        tm1.tm_mday = a.d;

        tm2.tm_year = b.y - 1900;
        tm2.tm_mon  = b.m - 1;
        tm2.tm_mday = b.d;

        time_t t1 = mktime(&tm1);
        time_t t2 = mktime(&tm2);
        if (t1 == (time_t)-1 || t2 == (time_t)-1) return -1;

        return (int)(difftime(t2, t1) / (60 * 60 * 24));
    }

    static string ajouterNuits(const string& date, int nuits) {
        DateParts a;
        if (!parseISODate(date, a)) return "";
        struct tm tm1 = {0};
        tm1.tm_isdst = -1;
        tm1.tm_year = a.y - 1900;
        tm1.tm_mon  = a.m - 1;
        tm1.tm_mday = a.d;

        tm1.tm_mday += nuits;
        if (mktime(&tm1) == (time_t)-1) return "";

        char buffer[11];
        strftime(buffer, 11, "%Y-%m-%d", &tm1);
        return string(buffer);
    }
};

string lireDateValide(const string& prompt) {
    while (true) {
        string raw = lireLigne(prompt);
        string formatted = BookingValidator::formaterDate(raw);
        if (!formatted.empty()) return formatted;
        cout << RED << "Date invalide. Exemple: 2026-01-03\n" << RESET;
    }
}

// ================== RESERVATION MANAGER ==================
class ReservationManager {
private:
    vector<Reservation> reservations;
    vector<Reservation> historiqueReservations;
    vector<Client> clients;
    int compteur = 1;
    int compteurClient = 1;

    bool datesChevauchent(const string& d1, const string& f1, const string& d2, const string& f2) const {
        return !(f1 <= d2 || f2 <= d1);
    }

    int trouverOuCreerClient(const string& nom, int age, const string& tel) {
        for (auto& c : clients) {
            if (c.telephone == tel) {
                c.nombreReservations++;
                sauvegarderClients(); // Update file after change
                return c.id;
            }
        }
        Client newClient(compteurClient++, nom, age, tel);
        newClient.nombreReservations = 1;
        clients.push_back(newClient);
        sauvegarderClients(); // Update file after addition
        return newClient.id;
    }

public:
    bool chambreDisponible(int numero, const string& debut, const string& fin) const {
        for (auto& r : reservations) {
            if (r.numeroChambre == numero) {
                if (datesChevauchent(r.dateDebut, r.dateFin, debut, fin))
                    return false;
            }
        }
        return true;
    }

    void afficherDisponibilites(const vector<Chambre>& chambres) const {
        string debut = lireDateValide("Date debut (YYYY-MM-DD) : ");
        string fin = lireDateValide("Date fin   (YYYY-MM-DD) : ");

        if (!BookingValidator::verifierDates(debut, fin)) {
            cout << RED << "Dates invalides\n" << RESET;
            return;
        }

        cout << "\nChambres disponibles :\n";
        cout << left << setw(10) << "Chambre" << setw(25) << "Type" << setw(10) << "Prix" << "\n";
        cout << string(45, '-') << "\n";

        int occupied = 0;
        bool trouve = false;
        for (auto& c : chambres) {
            if (chambreDisponible(c.numero, debut, fin)) {
                cout << left << setw(10) << c.numero << setw(25) << c.type << c.prix << " DH\n";
                trouve = true;
            } else {
                occupied++;
            }
        }
        if (!trouve)
            cout << YELLOW << "Aucune chambre disponible\n" << RESET;
        else if (occupied > 0)
            cout << YELLOW << occupied << " chambre(s) indisponible(s) pour ces dates.\n" << RESET;
    }

    void creerReservation(const vector<Chambre>& chambres,
                          const vector<ServiceHotel>& servicesHotel) {
        string nom, tel, debut;
        int age, nuits;

        nom = lireLigne("Nom du client : ");

        cout << "Age : ";
        while (!(cin >> age) || age <= 0) {
            cout << RED << "Age invalide. Reessayez : " << RESET;
            nettoyerCin();
        }
        nettoyerCin(); // Vider le buffer apres l'age

        tel = lireLigne("Telephone : ");

        debut = lireDateValide("Date d'arrivee (YYYY-MM-DD) : ");

        cout << "Nombre de nuits : ";
        while (!(cin >> nuits) || nuits <= 0) {
            cout << RED << "Nombre de nuits invalide. Reessayez : " << RESET;
            nettoyerCin();
        }

        string fin = BookingValidator::ajouterNuits(debut, nuits);
        cout << "Date de depart : " << fin << "\n";

        vector<const Chambre*> availableRooms;
        for (const auto& c : chambres) {
            if (chambreDisponible(c.numero, debut, fin)) {
                availableRooms.push_back(&c);
            }
        }

        if (availableRooms.empty()) {
            cout << RED << "Aucune chambre disponible.\n" << RESET;
            return;
        }

        int selection = 0;
        while (true) {
            clearScreen();
            header();
            cout << "Client : " << nom << " | " << debut << " -> " << fin << " (" << nuits << " nuits)\n";
            cout << string(60, '-') << "\n";
            cout << "CHOIX DE LA CHAMBRE (Utilisez les fleches + Entree) :\n\n";

            if (availableRooms.size() < chambres.size())
                cout << YELLOW << (chambres.size() - availableRooms.size()) << " chambre(s) indisponible(s).\n" << RESET;

            cout << left << setw(10) << "Chambre" << setw(25) << "Type" << setw(10) << "Prix" << "\n";
            cout << string(45, '-') << "\n";

            for (size_t i = 0; i < availableRooms.size(); ++i) {
                const auto* c = availableRooms[i];
                if (i == selection) {
                    cout << GREEN << " -> " << left << setw(6) << c->numero << setw(25) << c->type << c->prix << " DH <-\n" << RESET;
                } else {
                    cout << "    " << left << setw(6) << c->numero << setw(25) << c->type << c->prix << " DH\n";
                }
            }

#ifdef _WIN32
            int key = _getch();
            if (key == 224) {
                key = _getch();
                if (key == 72 && selection > 0) selection--;
                if (key == 80 && selection < (int)availableRooms.size() - 1) selection++;
            } else if (key == 13) {
                break;
            }
#else
            int temp;
            cout << "Entrez le numero : ";
            cin >> temp;
            bool found = false;
            for(size_t i=0; i<availableRooms.size(); ++i) {
                if(availableRooms[i]->numero == temp) {
                    selection = i;
                    found = true;
                    break;
                }
            }
            if(found) break;
#endif
        }

        const Chambre* chambre = availableRooms[selection];
        int num = chambre->numero;

        int clientId = trouverOuCreerClient(nom, age, tel);

        // Demonstration du polymorphisme
        for (const auto& c : clients) {
            if (c.id == clientId) {
                presenterUtilisateur(c);
                break;
            }
        }

        Reservation r(compteur++, clientId, num, debut, fin, nuits, nom, age, tel, chambre->prix);

        double totalServices = 0;
        for (auto& s : servicesHotel) {
            char ch;
            cout << s.nom << " (" << s.prix << " DH) ? (o/n) : ";
            cin >> ch;
            if (ch == 'o' || ch == 'O') {
                r.services.push_back(s);
                totalServices += s.prix;
            }
        }

        r.total = r.prixChambre * nuits + totalServices;

        int paiement;
        cout << "1 Cash  2 Carte : ";
        cout << "Mode de paiement (1: Cash, 2: Carte) : ";
        cin >> paiement;
        r.modePaiement = (paiement == 2) ? "Carte bancaire" : "Cash";

        reservations.push_back(r);
        sauvegarderDonnees(); // Update file after addition

        cout << GREEN << "\nTOTAL : " << r.total << " DH\n";
        cout << "Reservation N°" << r.id << " confirmee\n" << RESET;
    }

    void afficherReservations() const {
        if (reservations.empty()) {
            cout << YELLOW << "Aucune reservation\n" << RESET;
            return;
        }

        int termWidth = getTerminalWidth();
        cout << CYAN << string(termWidth, '-') << RESET << endl;
        string title = "Liste des reservations :";
        cout << CYAN << centerTextWithBorder(title, termWidth) << RESET << endl;
        cout << CYAN << string(termWidth, '-') << RESET << endl;

        int col1 = 5, col2 = 20, col3 = 10, col4 = 12, col5 = 12, col6 = 10;
        string border = string(col1 + col2 + col3 + col4 + col5 + col6 + 7, '-');
        vector<string> tableLines;
        tableLines.push_back(border);
        {
            stringstream ss;
            ss << "|" << left << setw(col1) << "ID" << "|" << left << setw(col2) << "Client" << "|" << left << setw(col3) << "Chambre" << "|" << left << setw(col4) << "Arrivee" << "|" << left << setw(col5) << "Depart" << "|" << left << setw(col6) << "Total" << "|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);
        for (auto& r : reservations) {
            stringstream ss;
            ss << "|" << left << setw(col1) << r.id
               << "|" << left << setw(col2) << r.nomClient.substr(0, 19)
               << "|" << left << setw(col3) << r.numeroChambre
               << "|" << left << setw(col4) << r.dateDebut
               << "|" << left << setw(col5) << r.dateFin
               << "|" << left << setw(col6 - 3) << r.total << " DH|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);

        for (const auto& line : tableLines) {
            cout << centerTextWithBorder(line, termWidth) << endl;
        }
    }

    void annulerReservation() {
        afficherReservations();

        int cancelId;
        cout << "\nEntrez l'ID de la reservation a annuler (0 pour retour) : ";
        if (!(cin >> cancelId) || cancelId == 0) {
            nettoyerCin();
            return;
        }
        nettoyerCin();
        vector<int> ids = {cancelId};

        bool anyCancelled = false;
        for (int cancelId : ids) {
            auto it = reservations.begin();
            bool found = false;
            while (it != reservations.end()) {
                if (it->id == cancelId) {
                    it->estActive = false;
                    historiqueReservations.push_back(*it);
                    it = reservations.erase(it);
                    sauvegarderHistorique(); // Update file after annulation
                    sauvegarderDonnees();    // Update file after annulation
                    cout << GREEN << "Reservation N°" << cancelId << " annulee\n" << RESET;
                    found = true;
                    anyCancelled = true;
                } else {
                    ++it;
                }
            }
            if (!found) {
                cout << YELLOW << "Reservation N°" << cancelId << " introuvable\n" << RESET;
            }
        }
    }

    void rechercherReservations() const {
        string query = lireLigne("Entrez le nom du client ou ID : ");
        bool found = false;
        for (const auto& r : reservations) {
            if (to_string(r.id) == query || r.nomClient.find(query) != string::npos) {
                cout << "ID: " << r.id << ", Client: " << r.nomClient << ", Chambre: " << r.numeroChambre
                     << ", Arrivee: " << r.dateDebut << ", Depart: " << r.dateFin << ", Total: " << r.total << " DH\n";
                found = true;
            }
        }
        if (!found) cout << YELLOW << "Aucune reservation trouvee.\n" << RESET;
    }

    void modifierReservation() {
        int id;
        cout << "ID de la reservation a modifier : ";
        cin >> id;
        nettoyerCin();
        for (auto& r : reservations) {
            if (r.id == id) {
                cout << "Reservation trouvee pour " << r.nomClient << "\n";
                cout << "Modifier dates? (o/n) : ";
                char ch;
                cin >> ch;
                nettoyerCin();
                if (ch == 'o' || ch == 'O') {
                    string newDebut = lireDateValide("Nouvelle date d'arrivee (YYYY-MM-DD) : ");
                    cout << "Nombre de nuits : ";
                    int newNuits;
                    while (!(cin >> newNuits) || newNuits <= 0) {
                        cout << RED << "Nombre invalide. Reessayez : " << RESET;
                        nettoyerCin();
                    }
                    nettoyerCin();
                    string newFin = BookingValidator::ajouterNuits(newDebut, newNuits);
                    if (newFin.empty()) {
                        cout << RED << "Erreur de calcul de date.\n" << RESET;
                        return;
                    }
                    if (!chambreDisponible(r.numeroChambre, newDebut, newFin)) {
                        cout << RED << "Chambre non disponible aux nouvelles dates.\n" << RESET;
                        return;
                    }
                    r.dateDebut = newDebut;
                    r.dateFin = newFin;
                    r.nuits = newNuits;
                    double totalServices = 0;
                    for (auto& s : r.services) totalServices += s.prix;
                    r.total = r.prixChambre * r.nuits + totalServices;
                    cout << GREEN << "Reservation modifiee.\n" << RESET;

                    // Auto-save immediately
                    sauvegarderDonnees();
                }
                return;
            }
        }
        cout << RED << "Reservation non trouvee.\n" << RESET;
    }

    void afficherChambresLibres(const vector<Chambre>& chambres) const {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char dateActuelle[11];
        strftime(dateActuelle, 11, "%Y-%m-%d", ltm);
        string aujourdhui(dateActuelle);
        string demain = BookingValidator::ajouterNuits(aujourdhui, 1);

        int termWidth = getTerminalWidth();
        cout << CYAN << string(termWidth, '-') << RESET << endl;
        string title = "Chambres libres aujourd'hui :";
        cout << CYAN << centerTextWithBorder(title, termWidth) << RESET << endl;
        cout << CYAN << string(termWidth, '-') << RESET << endl;

        int col1 = 10, col2 = 25, col3 = 12;
        string border = string(col1 + col2 + col3 + 4, '-');
        vector<string> tableLines;
        tableLines.push_back(border);
        {
            stringstream ss;
            ss << "|" << left << setw(col1) << "Chambre" << "|" << left << setw(col2) << "Type" << "|" << left << setw(col3) << "Prix" << "|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);
        int occupied = 0;
        for (const auto& c : chambres) {
            if (chambreDisponible(c.numero, aujourdhui, demain)) {
                stringstream ss;
                ss << "|" << left << setw(col1) << c.numero << "|" << left << setw(col2) << c.type << "|" << left << setw(col3 - 3) << c.prix << " DH|";
                tableLines.push_back(ss.str());
            } else {
                occupied++;
            }
        }
        tableLines.push_back(border);

        for (const auto& line : tableLines) {
            cout << centerTextWithBorder(line, termWidth) << endl;
        }
        if (occupied > 0)
            cout << YELLOW << centerTextWithBorder(to_string(occupied) + " chambre(s) occupee(s) aujourd'hui.", termWidth) << RESET << endl;
    }

    void afficherHistorique() const {
        if (historiqueReservations.empty()) {
            int termWidth = getTerminalWidth();
            cout << YELLOW << centerTextWithBorder("Aucun historique", termWidth) << RESET << endl;
            return;
        }

        int termWidth = getTerminalWidth();
        cout << CYAN << string(termWidth, '-') << RESET << endl;
        string title = "=== HISTORIQUE DES RESERVATIONS ===";
        cout << CYAN << centerTextWithBorder(title, termWidth) << RESET << endl;
        cout << CYAN << string(termWidth, '-') << RESET << endl;

        int col1 = 4, col2 = 15, col3 = 8, col4 = 11, col5 = 11, col6 = 22;
        string border = string(col1 + col2 + col3 + col4 + col5 + col6 + 7, '-');
        vector<string> tableLines;
        tableLines.push_back(border);
        {
            stringstream ss;
            ss << "|" << left << setw(col1) << "ID" << "|" << left << setw(col2) << "Client" << "|" << left << setw(col3) << "Chambre" << "|" << left << setw(col4) << "Arrivee" << "|" << left << setw(col5) << "Depart" << "|" << left << setw(col6) << "Total" << "|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);
        for (auto& r : historiqueReservations) {
            stringstream ss;
            stringstream ssTotal;
            ssTotal << r.total << " DH [ANNULEE]";
            ss << "|" << left << setw(col1) << r.id
               << "|" << left << setw(col2) << (r.nomClient.length() > (size_t)col2-1 ? r.nomClient.substr(0, col2-1) : r.nomClient)
               << "|" << left << setw(col3) << r.numeroChambre
               << "|" << left << setw(col4) << r.dateDebut
               << "|" << left << setw(col5) << r.dateFin
               << "|" << left << setw(col6) << ssTotal.str() << "|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);

        for (const auto& line : tableLines) {
            cout << centerTextWithBorder(line, termWidth) << endl;
        }
    }

    void afficherClients() const {
        if (clients.empty()) {
            cout << YELLOW << "Aucun client enregistre\n" << RESET;
            return;
        }

        int termWidth = getTerminalWidth();

        int col1 = 5, col2 = 25, col3 = 15, col4 = 5;
        string border = string(col1 + col2 + col3 + col4 + 5, '-');
        vector<string> tableLines;
        tableLines.push_back(border);
        {
            stringstream ss;
            ss << "|" << left << setw(col1) << "ID" << "|" << left << setw(col2) << "Nom" << "|" << left << setw(col3) << "Telephone" << "|" << left << setw(col4) << "Age" << "|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);
        for (const auto& c : clients) {
            stringstream ss;
            ss << "|" << left << setw(col1) << c.id
               << "|" << left << setw(col2) << c.nom.substr(0, 24)
               << "|" << left << setw(col3) << c.telephone
               << "|" << left << setw(col4) << c.age << "|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);

        for (const auto& line : tableLines) {
            cout << centerTextWithBorder(line, termWidth) << endl;
        }
    }

    void ajouterSignalement() {
        int termWidth = getTerminalWidth();
        cout << CYAN << centerTextWithBorder("--- SIGNALER UN PROBLEME / AVIS ---", termWidth) << RESET << endl;

        string nom = lireLigne("Votre nom : ");
        string sujet = lireLigne("Sujet : ");
        string contenu = lireLigne("Message : ");

        ofstream fichier("rapports.txt", ios::app);
        if (fichier) {
            time_t now = time(0);
            char* dt = ctime(&now);
            string dateStr(dt);
            if (!dateStr.empty()) dateStr.pop_back();

            fichier << "========================================\n";
            fichier << "TYPE: SIGNALEMENT CLIENT\n";
            fichier << "DATE: " << dateStr << "\n";
            fichier << "CLIENT: " << nom << "\n";
            fichier << "SUJET: " << sujet << "\n";
            fichier << "CONTENU:\n" << contenu << "\n";
            fichier << "========================================\n\n";
            cout << GREEN << "Votre message a ete enregistre. Merci.\n" << RESET;
        } else {
            cout << RED << "Erreur systeme.\n" << RESET;
        }
    }

    void afficherRapports() const {
        ifstream fichier("rapports.txt");
        if (!fichier) {
            int termWidth = getTerminalWidth();
            cout << YELLOW << centerTextWithBorder("Aucun rapport trouve.", termWidth) << RESET << endl;
            return;
        }

        struct Rapport {
            string date;
            string client;
            string sujet;
            string contenu;
        };
        vector<Rapport> rapports;
        Rapport temp;
        string ligne;
        bool readingContent = false;

        while (getline(fichier, ligne)) {
            if (ligne.find("========================================") != string::npos) {
                if (readingContent) {
                    rapports.push_back(temp);
                    temp = Rapport();
                    readingContent = false;
                }
                continue;
            }
            if (ligne.find("DATE: ") == 0) temp.date = ligne.substr(6);
            else if (ligne.find("CLIENT: ") == 0) temp.client = ligne.substr(8);
            else if (ligne.find("SUJET: ") == 0) temp.sujet = ligne.substr(7);
            else if (ligne == "CONTENU:") readingContent = true;
            else if (readingContent && !ligne.empty()) {
                if (!temp.contenu.empty()) temp.contenu += " ";
                temp.contenu += ligne;
            }
        }
        fichier.close();

        int termWidth = getTerminalWidth();
        cout << CYAN << string(termWidth, '-') << RESET << endl;
        cout << CYAN << centerTextWithBorder("=== RAPPORTS ET SIGNALEMENTS ===", termWidth) << RESET << endl;
        cout << CYAN << string(termWidth, '-') << RESET << endl;

        if (rapports.empty()) {
            cout << YELLOW << centerTextWithBorder("Aucun rapport a afficher.", termWidth) << RESET << endl;
            return;
        }

        int col1 = 25, col2 = 15, col3 = 20, col4 = 30;
        string border = string(col1 + col2 + col3 + col4 + 5, '-');
        vector<string> tableLines;
        tableLines.push_back(border);
        {
            stringstream ss;
            ss << "|" << left << setw(col1) << "Date" << "|" << left << setw(col2) << "Client" << "|" << left << setw(col3) << "Sujet" << "|" << left << setw(col4) << "Contenu" << "|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);

        for (const auto& r : rapports) {
            stringstream ss;
            string c = (r.client.length() > (size_t)col2 - 1) ? r.client.substr(0, col2 - 4) + "..." : r.client;
            string s = (r.sujet.length() > (size_t)col3 - 1) ? r.sujet.substr(0, col3 - 4) + "..." : r.sujet;
            string co = (r.contenu.length() > (size_t)col4 - 1) ? r.contenu.substr(0, col4 - 4) + "..." : r.contenu;

            ss << "|" << left << setw(col1) << r.date.substr(0, col1 - 1)
               << "|" << left << setw(col2) << c
               << "|" << left << setw(col3) << s
               << "|" << left << setw(col4) << co << "|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);

        for (const auto& line : tableLines) {
            cout << centerTextWithBorder(line, termWidth) << endl;
        }
    }

    void reservationHistorique() const {
        double totalRevenue = 0;
        int totalReservations = reservations.size();
        map<int, int> roomUsage;
        for (const auto& r : reservations) {
            totalRevenue += r.total;
            roomUsage[r.numeroChambre]++;
        }

        int termWidth = getTerminalWidth();
        cout << CYAN << string(termWidth, '-') << RESET << endl;
        string title = "=== RAPPORT RESERVATIONS ===";
        cout << CYAN << centerTextWithBorder(title, termWidth) << RESET << endl;
        cout << CYAN << string(termWidth, '-') << RESET << endl;

        // Table for summary
        int col1 = 25, col2 = 25;
        string border = string(col1 + col2 + 3, '-');
        vector<string> tableLines;
        tableLines.push_back(border);
        {
            stringstream ss;
            ss << "|" << left << setw(col1) << "Total reservations" << "|" << left << setw(col2) << totalReservations << "|";
            tableLines.push_back(ss.str());
        }
        {
            stringstream ss;
            ss << "|" << left << setw(col1) << "Revenus totaux" << "|" << left << setw(col2 - 3) << totalRevenue << " DH|";
            tableLines.push_back(ss.str());
        }
        tableLines.push_back(border);

        for (const auto& line : tableLines) {
            cout << centerTextWithBorder(line, termWidth) << endl;
        }

        // Table for room usage
        cout << endl;
        string usageTitle = "Utilisation des chambres";
        cout << CYAN << centerTextWithBorder(usageTitle, termWidth) << RESET << endl;
        string usageBorder = string(20 + 20 + 3, '-');
        vector<string> usageLines;
        usageLines.push_back(usageBorder);
        {
            stringstream ss;
            ss << "|" << left << setw(20) << "Chambre" << "|" << left << setw(20) << "Reservations" << "|";
            usageLines.push_back(ss.str());
        }
        usageLines.push_back(usageBorder);
        for (auto& p : roomUsage) {
            stringstream ss;
            ss << "|" << left << setw(20) << p.first << "|" << left << setw(20) << p.second << "|";
            usageLines.push_back(ss.str());
        }
        usageLines.push_back(usageBorder);
        for (const auto& line : usageLines) {
            cout << centerTextWithBorder(line, termWidth) << endl;
        }

        cout << endl;
        cout << CYAN << centerTextWithBorder("--- LISTE DES CLIENTS ---", termWidth) << RESET << endl;
        afficherClients();

        cout << "\nSouhaitez-vous ajouter un rapport textuel ? (o/n) : ";
        char ch;
        cin >> ch;
        if (ch == 'o' || ch == 'O') {
            nettoyerCin();
            string sujet = lireLigne("Sujet : ");
            string contenu = lireLigne("Rapport : ");

            ofstream fichier("rapports.txt", ios::app);
            if (fichier) {
                time_t now = time(0);
                char* dt = ctime(&now);
                string dateStr(dt);
                if (!dateStr.empty()) dateStr.pop_back();

                fichier << "========================================\n";
                fichier << "DATE: " << dateStr << "\n";
                fichier << "SUJET: " << sujet << "\n";
                fichier << "CONTENU:\n" << contenu << "\n";
                fichier << "========================================\n\n";
                cout << GREEN << "Rapport sauvegarde dans 'rapports.txt'.\n" << RESET;
            } else {
                cout << RED << "Erreur d'ouverture du fichier rapports.txt\n" << RESET;
            }
        }
        else {
            nettoyerCin();
        }
    }

    void sauvegarderDonnees() const {
        ofstream fichier("reservations.txt");
        if (!fichier) return;

        for (const auto& r : reservations) {
            fichier << r.id << "\n"
                    << r.clientId << "\n"
                    << r.numeroChambre << "\n"
                    << r.dateDebut << "\n"
                    << r.dateFin << "\n"
                    << r.nuits << "\n"
                    << r.total << "\n"
                    << r.nomClient << "\n"
                    << r.ageClient << "\n"
                    << r.telephoneClient << "\n"
                    << r.prixChambre << "\n"
                    << r.modePaiement << "\n"
                    << r.services.size() << "\n";
            for (const auto& s : r.services) {
                fichier << s.nom << "\n" << s.prix << "\n";
            }
        }
        cout << GREEN << "Reservations sauvegardees\n" << RESET;
    }

    void sauvegarderClients() const {
        ofstream fichier("clients.txt");
        if (!fichier) return;

        for (const auto& c : clients) {
            fichier << c.id << "\n"
                    << c.nom << "\n"
                    << c.age << "\n"
                    << c.telephone << "\n"
                    << c.email << "\n"
                    << c.nombreReservations << "\n";
        }
    }

    void sauvegarderHistorique() const {
        ofstream fichier("historique.txt");
        if (!fichier) return;

        for (const auto& r : historiqueReservations) {
            fichier << r.id << "\n"
                    << r.clientId << "\n"
                    << r.numeroChambre << "\n"
                    << r.dateDebut << "\n"
                    << r.dateFin << "\n"
                    << r.nuits << "\n"
                    << r.total << "\n"
                    << r.nomClient << "\n"
                    << r.ageClient << "\n"
                    << r.telephoneClient << "\n"
                    << r.prixChambre << "\n"
                    << r.modePaiement << "\n"
                    << r.estActive << "\n"
                    << r.services.size() << "\n";
            for (const auto& s : r.services) {
                fichier << s.nom << "\n" << s.prix << "\n";
            }
        }
    }

    void sauvegarderChambresLibres(const vector<Chambre>& chambres) const {
        ofstream fichier("chambres_libres.txt");
        if (!fichier) return;

        time_t now = time(0);
        tm* ltm = localtime(&now);
        char dateActuelle[11];
        strftime(dateActuelle, 11, "%Y-%m-%d", ltm);
        string aujourdhui(dateActuelle);
        string demain = BookingValidator::ajouterNuits(aujourdhui, 1);

        fichier << "Chambres libres le " << aujourdhui << "\n";
        fichier << "Numero\tType\tPrix\n";

        for (const auto& c : chambres) {
            if (chambreDisponible(c.numero, aujourdhui, demain)) {
                fichier << c.numero << "\t" << c.type << "\t" << c.prix << "\n";
            }
        }
    }

    void chargerReservations() {
        ifstream fichier("reservations.txt");
        if (!fichier) return;

        reservations.clear();
        int id, clientId, num, nuits, nbServices;
        string d1, d2, mode, sNom, nom, tel;
        int age;
        double total, sPrix, prixChambre;

        while (fichier >> id) {
            fichier >> clientId >> num >> d1 >> d2 >> nuits >> total;
            fichier.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(fichier, nom);
            fichier >> age;
            fichier.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(fichier, tel);
            fichier >> prixChambre;
            fichier.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(fichier, mode);
            fichier >> nbServices;
            fichier.ignore(numeric_limits<streamsize>::max(), '\n');

            Reservation r(id, clientId, num, d1, d2, nuits, nom, age, tel, prixChambre);
            r.total = total;
            r.modePaiement = mode;

            for (int i = 0; i < nbServices; ++i) {
                getline(fichier, sNom);
                fichier >> sPrix;
                fichier.ignore(numeric_limits<streamsize>::max(), '\n');
                r.services.push_back(ServiceHotel(sNom, sPrix));
            }
            reservations.push_back(r);
            if (id >= compteur) compteur = id + 1;
        }
        cout << GREEN << "Reservations chargees (" << reservations.size() << ")\n" << RESET;
        pause(500);
    }

    void chargerClients() {
        ifstream fichier("clients.txt");
        if (!fichier) return;

        clients.clear();
        int id, age, nbRes;
        string nom, tel, email;

        while (fichier >> id) {
            fichier.ignore();
            getline(fichier, nom);
            fichier >> age;
            fichier.ignore();
            getline(fichier, tel);
            getline(fichier, email);
            fichier >> nbRes;

            Client c(id, nom, age, tel, email);
            c.nombreReservations = nbRes;
            clients.push_back(c);
            if (id >= compteurClient) compteurClient = id + 1;
        }
        cout << GREEN << "Clients charges (" << clients.size() << ")\n" << RESET;
        pause(500);
    }

    void chargerHistorique() {
        ifstream fichier("historique.txt");
        if (!fichier) return;

        historiqueReservations.clear();
        int id, clientId, num, nuits, nbServices;
        string d1, d2, mode, sNom, nom, tel;
        int age;
        bool active;
        double total, sPrix, prixChambre;

        while (fichier >> id) {
            fichier >> clientId >> num >> d1 >> d2 >> nuits >> total;
            fichier.ignore();
            getline(fichier, nom);
            fichier >> age;
            fichier.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(fichier, tel);
            fichier >> prixChambre;
            fichier.ignore();
            getline(fichier, mode);
            fichier >> active >> nbServices;
            fichier.ignore();

            Reservation r(id, clientId, num, d1, d2, nuits, nom, age, tel, prixChambre);
            r.total = total;
            r.modePaiement = mode;
            r.estActive = active;

            for (int i = 0; i < nbServices; ++i) {
                getline(fichier, sNom);
                fichier >> sPrix;
                fichier.ignore();
                r.services.push_back(ServiceHotel(sNom, sPrix));
            }
            historiqueReservations.push_back(r);
        }
        cout << GREEN << "Historique charge (" << historiqueReservations.size() << ")\n" << RESET;
        pause(500);
    }
};

// ================== ADMIN ==================
class Admin : public Personne {
private:
    string username;
    string password;

public:
    Admin() {
        ifstream file("admin_config.txt");
        if (file) {
            file >> username >> password;
        } else {
            username = "admin";
            password = "1234";
        }
    }

    bool authentifier() {
        string l, p;
        cout << "Login : ";
        cin >> l;
        cout << "Mot de passe : ";
        cin >> p;
        return l == username && p == password;
    }

    void changerIdentifiants() {
        cout << "Nouveau login : ";
        cin >> username;
        cout << "Nouveau mot de passe : ";
        cin >> password;

        ofstream file("admin_config.txt");
        file << username << " " << password;

        cout << GREEN << "Identifiants modifies avec succes.\n" << RESET;
    }

    string getRole() const override { return "Administrateur"; }

    void afficher() const override {
        cout << "Admin: " << username << "\n";
    }
};

// ================== MAIN ==================
// ================== CHARGEMENT FICHIERS ==================
vector<Chambre> chargerChambres() {
    vector<Chambre> chambres;
    ifstream fichier("chambres.txt");
    if (!fichier) {
        cout << YELLOW << "Fichier chambres.txt introuvable. Creation avec donnees par defaut...\n" << RESET;
        ofstream out("chambres.txt");
        out << "101\nStandard\n950\n";
        out << "102\nJunior\n650\n";
        out << "201\nVIP\n9000\n";
        out << "207\nBungalow prive\n15000\n";
        out << "250\nSuite presidentielle\n18000\n";
        out << "301\nSuite Royale\n8000\n";
        out << "209\nDeluxe\n5000\n";
        out.close();
        fichier.open("chambres.txt");
    }

    int num;
    string type;
    double prix;
    while (fichier >> num) {
        fichier.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(fichier, type);
        if (!type.empty() && type.back() == '\r') type.pop_back();
        fichier >> prix;
        fichier.ignore(numeric_limits<streamsize>::max(), '\n');
        chambres.push_back(Chambre(num, type, prix));
    }
    cout << GREEN << "Chambres chargees (" << chambres.size() << ")\n" << RESET;
    pause(500);
    return chambres;
}

vector<ServiceHotel> chargerServices() {
    vector<ServiceHotel> services;
    ifstream fichier("services.txt");
    if (!fichier) {
        cout << YELLOW << "Fichier services.txt introuvable. Creation avec donnees par defaut...\n" << RESET;
        ofstream out("services.txt");
        out << "Spa\n300\n";
        out << "Restaurant\n700\n";
        out << "Piscine privee\n250\n";
        out << "Minibar\n850\n";
        out << "Pressing\n125\n";
        out << "Nettoyage express\n450\n";
        out << "Baby-sitting\n350\n";
        out.close();
        fichier.open("services.txt");
    }

    string nom;
    double prix;
    while (getline(fichier, nom)) {
        if (!nom.empty() && nom.back() == '\r') nom.pop_back();
        if (fichier >> prix) {
            fichier.ignore(numeric_limits<streamsize>::max(), '\n');
            services.push_back(ServiceHotel(nom, prix));
        }
    }
    cout << GREEN << "Services charges (" << services.size() << ")\n" << RESET;
    pause(500);
    return services;
}

void sauvegarderChambres(const vector<Chambre>& chambres) {
    ofstream out("chambres.txt");
    if (!out) return;
    for (const auto& c : chambres) {
        out << c.numero << "\n";
        out << c.type << "\n";
        out << c.prix << "\n";
    }
}

void sauvegarderServices(const vector<ServiceHotel>& services) {
    ofstream out("services.txt");
    if (!out) return;
    for (const auto& s : services) {
        out << s.nom << "\n";
        out << s.prix << "\n";
    }
}

int main() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(hOut, dwMode);
#endif
    loadingAnimation("The Town");
    pause(800);

    vector<Chambre> chambres = chargerChambres();
    vector<ServiceHotel> services = chargerServices();

    ReservationManager manager;
    Admin admin;
    manager.chargerReservations();
    manager.chargerClients();
    manager.chargerHistorique();

    int choix;
    do {
        choix = menuPrincipal();

        switch (choix) {
        case 1:
            header();
            manager.creerReservation(chambres, services);
            break;
        case 2:
            header();
            manager.afficherChambresLibres(chambres);
            manager.sauvegarderChambresLibres(chambres);
            break;
        case 3:
            header();
            afficherInfoHotel();
            break;
        case 4:
            header();
            manager.ajouterSignalement();
            break;
        case 5:
            header();
            if (admin.authentifier()) {
                presenterUtilisateur(admin);
                pause(1500);
                vector<string> adminOptions = {
                    "Nouvelle reservation",
                    "Voir chambres libres",
                    "Infos hotel",
                    "Edit hotel info",
                    "Afficher chambres",
                    "Modifier prix TYPE chambre",
                    "Modifier prix SERVICE",
                    "Afficher reservations",
                    "Afficher clients",
                    "Annuler reservation",
                    "Afficher historique",
                    "Voir rapports",
                    "Deconnexion"
                };
                int adminSelection = 0;
                while (true) {

                        clearScreen();
                        header();
                        int termWidth = getTerminalWidth();
                        string border(termWidth, '=');
                        cout << MAGENTA << border << RESET << endl;
                        cout << centerTextWithBorder("=== MENU ADMIN ===", termWidth) << endl;
                        cout << MAGENTA << border << RESET << endl;
                        for (size_t i = 0; i < adminOptions.size(); ++i) {
                            string line = (i == adminSelection) ? ("-> " + adminOptions[i] + " <-") : adminOptions[i];
                            string color = (i == adminSelection) ? GREEN : RESET;
                            cout << color << centerTextWithBorder(line, termWidth) << RESET << endl;
                        }
                        cout << MAGENTA << border << RESET << endl;

                    int adminChoix = -1;
                    #ifdef _WIN32
                    int key = _getch();
                    if (key == 224) {
                        key = _getch();
                        if (key == 72 && adminSelection > 0) adminSelection--;
                        if (key == 80 && adminSelection < (int)adminOptions.size() - 1) adminSelection++;
                        continue;
                    } else if (key == 13) {
                        adminChoix = (adminSelection == (int)adminOptions.size() - 1) ? 0 : adminSelection + 1;
                    } else {
                        continue;
                    }
                    #else
                    cout << "Votre choix : ";
                    cin >> adminChoix;
                    if(cin.peek() == '\n') cin.ignore();
                    #endif

                    if (adminChoix == 0) break;

                    if (adminChoix == 1) {
                        header();
                        manager.creerReservation(chambres, services);
                    } else if (adminChoix == 2) {
                        header();
                        manager.afficherChambresLibres(chambres);
                        manager.sauvegarderChambresLibres(chambres);
                        cout << "\nAppuyez sur une touche...";
                        #ifdef _WIN32
                        _getch();
                        #else
                        cin.get();
                        #endif
                    } else if (adminChoix == 3) {
                        header();
                        afficherInfoHotel();
                        cout << "\nAppuyez sur une touche...";
                        #ifdef _WIN32
                        _getch();
                        #else
                        cin.get();
                        #endif
                    } else if (adminChoix == 4) {
                        // Edit hotel info
                        header();
                        string filename = "hotel_info.txt";
                        vector<string> fields = {
                            "Nom de l'hotel",
                            "Adresse",
                            "Telephone",
                            "Email",
                            "Check-in/Check-out"
                        };
                        vector<string> values(fields.size());
                        cout << CYAN << "=== EDIT HOTEL INFO ===" << RESET << endl;
                        for (size_t i = 0; i < fields.size(); ++i) {
                            cout << fields[i] << ": ";
                            getline(cin >> ws, values[i]);
                        }
                        ofstream out(filename);
                        if (out) {
                            out << values[0] << "\n";
                            out << "Adresse: " << values[1] << "\n";
                            out << "Telephone: " << values[2] << "\n";
                            out << "Email: " << values[3] << "\n";
                            out << "Check-in: " << values[4] << "\n";
                            out.close();
                            cout << GREEN << "Infos hotel mises a jour!" << RESET << endl;
                        } else {
                            cout << RED << "Erreur d'ecriture hotel_info.txt" << RESET << endl;
                        }
                        cout << "\nAppuyez sur une touche...";
                        #ifdef _WIN32
                        _getch();
                        #else
                        cin.get();
                        #endif
                    } else if (adminChoix == 5) {
                        int termWidth = getTerminalWidth();
                        cout << CYAN << string(termWidth, '-') << RESET << endl;
                        string title = "Liste des chambres :";
                        cout << CYAN << centerTextWithBorder(title, termWidth) << RESET << endl;
                        cout << CYAN << string(termWidth, '-') << RESET << endl;

                        int col1 = 10, col2 = 25, col3 = 12;
                        string border = string(col1 + col2 + col3 + 4, '-');
                        vector<string> tableLines;
                        tableLines.push_back(border);
                        {
                            stringstream ss;
                            ss << "|" << left << setw(col1) << "Chambre" << "|" << left << setw(col2) << "Type" << "|" << left << setw(col3) << "Prix" << "|";
                            tableLines.push_back(ss.str());
                        }
                        tableLines.push_back(border);
                        for (const auto& c : chambres) {
                            stringstream ss;
                            ss << "|" << left << setw(col1) << c.numero << "|" << left << setw(col2) << c.type << "|" << left << setw(col3 - 3) << c.prix << " DH|";
                            tableLines.push_back(ss.str());
                        }
                        tableLines.push_back(border);

                        for (const auto& line : tableLines) {
                            cout << centerTextWithBorder(line, termWidth) << endl;
                        }
                        cout << "\nAppuyez sur une touche...";
                        #ifdef _WIN32
                        _getch();
                        #else
                        cin.get();
                        #endif
                    } else if (adminChoix == 6) {
                        vector<string> types;
                        for (const auto& c : chambres) {
                            bool exists = false;
                            for (const auto& t : types) if (t == c.type) exists = true;
                            if (!exists) types.push_back(c.type);
                        }

                        int typeSel = 0;
                        string selectedType;
                        while (true) {
                            clearScreen();
                            header();
                            cout << CYAN << "=== CHOISIR TYPE ===\n" << RESET;
                            for (size_t i = 0; i < types.size(); ++i) {
                                if (i == typeSel) cout << GREEN << " -> " << types[i] << " <-\n" << RESET;
                                else cout << "    " << types[i] << "\n";
                            }

                            #ifdef _WIN32
                            int key = _getch();
                            if (key == 224) {
                                key = _getch();
                                if (key == 72 && typeSel > 0) typeSel--;
                                if (key == 80 && typeSel < (int)types.size() - 1) typeSel++;
                            } else if (key == 13) {
                                selectedType = types[typeSel];
                                break;
                            }
                            #else
                            cout << "Type : "; cin >> ws; getline(cin, selectedType); break;
                            #endif
                        }

                        double newP;
                        cout << "\nNouveau prix pour " << selectedType << " : ";
                        cin >> newP;
                        for(auto& c : chambres) {
                            if(c.type == selectedType) {
                                c.prix = newP;
                            }
                        }
                        sauvegarderChambres(chambres);
                        cout << GREEN << "Prix mis a jour.\n" << RESET;
                        pause(1000);
                    } else if (adminChoix == 7) {
                        cout << "--- Services ---\n";
                        for(size_t i=0; i<services.size(); ++i)
                            cout << i+1 << ". " << services[i].nom << " (" << services[i].prix << " DH)\n";

                        int sIdx;
                        cout << "Numero du service : ";
                        cin >> sIdx;
                        if(sIdx > 0 && sIdx <= (int)services.size()) {
                            double newP;
                            cout << "Nouveau prix : ";
                            cin >> newP;
                            services[sIdx-1].prix = newP;
                            sauvegarderServices(services);
                            cout << GREEN << "Prix mis a jour.\n" << RESET;
                        } else {
                            cout << RED << "Service invalide.\n" << RESET;
                        }
                        pause(1000);
                    } else if (adminChoix == 8) {
                        manager.afficherReservations();
                        cout << "\nAppuyez sur une touche...";
                        #ifdef _WIN32
                        _getch();
                        #else
                        cin.get();
                        #endif
                    } else if (adminChoix == 9) {
                        manager.afficherClients();
                        cout << "\nAppuyez sur une touche...";
                        #ifdef _WIN32
                        _getch();
                        #else
                        cin.get();
                        #endif
                    } else if (adminChoix == 10) {
                        manager.annulerReservation();
                        pause(1000);
                    } else if (adminChoix == 11) {
                        manager.afficherHistorique();
                        cout << "\nAppuyez sur une touche...";
                        #ifdef _WIN32
                        _getch();
                        #else
                        cin.get();
                        #endif
                    } else if (adminChoix == 12) {
                        manager.afficherRapports();
                        cout << "\nAppuyez sur une touche...";
                        #ifdef _WIN32
                        _getch();
                        #else
                        cin.get();
                        #endif
                    }
                }
            } else {
                cout << RED << "Acces refuse\n" << RESET;
            }
            break;
        case 0:
            cout << MAGENTA << "Merci d'avoir utilise le systeme\n" << RESET;
            manager.sauvegarderDonnees();
            manager.sauvegarderClients();
            manager.sauvegarderHistorique();
            break;
        default:
            cout << "Choix invalide\n";
        }

        if (choix != 0) {
            cout << "\nAppuyez sur une touche pour continuer...";
#ifdef _WIN32
            _getch();
#else
            nettoyerCin();
            cin.get();
#endif
        }

    } while (choix != 0);

    return 0;
}
