#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stdexcept>

class GuessingSystemException : public std::exception {
public:
    GuessingSystemException(const std::string& message) : message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

class GuessingSystem {
private:
    std::vector<std::string> questions;  // Vektor u kome æe biti pitanja
    std::map<std::string, std::vector<std::string>> dataset; // Mapa u kojoj æe kljuèevi biti nazivi, a vrednost vektori sa osobinama

    void throwError(const std::string& errorMessage) const {
        throw GuessingSystemException(errorMessage);
    }

public:
    void openTerminal();
    void loadDataset(const std::string& filepath);
    void guess();
    void guessTry(int num_of_attempts);
    void displayInfo(const std::string& name);
    void addNewInstance(const std::vector<std::string>& instanceData, const std::string& instanceName, const std::string& filePath);
    void deleteInstance(const std::string& name, const std::string& filepath);
};

void GuessingSystem::openTerminal() {
    std::string command;
    std::string filepath;
    std::cout << "-------------------------\n";
    std::cout << "Welcome to Guessing Game!\n";
    std::cout << "-------------------------\n";

    while (true) {
        std::cout << "What should I do next?\n";
        std::cin >> command;

        if (command == "EXIT") {
            break;
        }
        else if (command == "LOAD") {
            std::cout << "Please enter the path to the file:\n";
            std::cin >> filepath;
            loadDataset(filepath);
        }
        else if (command == "GUESS") {
            guess();
        }
        else if (command == "TRY") {
            int num_of_attempts;
            std::cout << "Enter the number of attempts:\n";
            std::cin >> num_of_attempts;
            guessTry(num_of_attempts);
        }
        else if (command == "INFO") {
            std::string name;
            std::cout << "Enter the name of the instance:\n";
            std::cin >> name;
            displayInfo(name);
        }
        else if (command == "NEW") {
            std::vector<std::string> features; // Vektor sa osobinama
            std::string name;
            std::cout << "Enter features (comma-separated) and name:\n";
            std::string input;
            std::cin.ignore();
            std::getline(std::cin, input);
            std::istringstream iss(input);
            std::string feature;
            while (std::getline(iss, feature, ',')) {
                features.push_back(feature);
            }
            addNewInstance(features, features.back(), filepath);  // Poslednja osobina je ime
        }
        else if (command == "DELETE") {
            std::string name;
            std::cout << "Enter the name of the instance to delete:\n";
            std::cin >> name;
            deleteInstance(name, filepath);
        }
        else {
            std::cout << "Invalid command. Try again.\n";
        }
    }
}

void GuessingSystem::loadDataset(const std::string& filepath) {
    if (!questions.empty() || !dataset.empty()) { // Ako su question ili dateset nisu prazni, to znaèi da veæ postoji uèitan skup podataka
        std::cout << "Podaci su vec ucitani.\n";
        questions.clear();
        dataset.clear();
    }

    std::ifstream file(filepath);
    if (!file.is_open()) {
        throwError("Greska prilikom otvaranja fajla: " + filepath);
    }

    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string question;
        while (std::getline(iss, question, ',')) {
            questions.push_back(question); // Popunjavanje vektora sa pitanjima iz prve linije fajla
        }
    }
    else {
        throwError("Nevazeci format fajla.");
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string feature;
        std::vector<std::string> features;
        while (std::getline(iss, feature, ',')) {
            features.push_back(feature); // Za ostale linije se prvo popunjava vektor osobina
        }

        if (features.size() == questions.size() + 1) {
            std::string name = features.back(); // Ime se nalazi na kraju
            features.pop_back(); // Izbacuje se ime iz osobina
            dataset[name] = features; // Mapa dataset sa kljuèem imena i vrednosti vektora osobina
        }
        else {
            std::cerr << "Preskoci liniju.\n";
        }
    }

    std::cout << "Podaci uspesno procitani.\n";
    file.close();
}

void GuessingSystem::guess() {
    std::cout << "Pokusacu da pogodim.\n";

    std::set<std::string> remainingInstances; // Skup podataka sa preostalim imenima
    for (const auto& entry : dataset) {
        remainingInstances.insert(entry.first); // U poèetku sva imena
    }

    // Pitanja se postavljaju redosledom iz skupa pitanja
    for (int i = 0; i < questions.size(); ++i) {
        const std::string& question = questions[i];
        std::cout << "- " << question;

        std::set<std::string> uniqueAnswers;
        for (const auto& instance : remainingInstances) {
            if (!dataset.at(instance)[i].empty()) {
                uniqueAnswers.insert(dataset.at(instance)[i]);
            }
        }
        std::cout << "\n";

        // Provera da li postoji jedinstven odgovor
        if (uniqueAnswers.size() == 1) {
            std::cout << "To je " << *uniqueAnswers.begin() << "!\n";
            return;  // Prekini pogaðanje ako postoji jednoznaèan odgovor
        }

        // Unos odgovora od strane korisnika
        std::string userAnswer;
        std::cin >> userAnswer;

        // Filtriranje skupa preostalih instanci
        std::set<std::string> newRemainingInstances; // Novi skup za imena
        for (const auto& instance : remainingInstances) {
            if (dataset.at(instance)[i] == userAnswer) { // Ako je uneti odgovor isti kao osobina, u novi skup podataka ce ici sva imena sa tom osobinom
                newRemainingInstances.insert(instance);
            }
        }
        remainingInstances = newRemainingInstances;

        if (remainingInstances.empty()) {
            std::cout << "Ne mogu da pogodim.\n";
            return;
        }

        // Provera da li su preostala pitanja potrebna
        if (remainingInstances.size() == 1) {
            std::cout << "It's " << *remainingInstances.begin() << "!\n";
            return;
        }
    }

    // Ispisivanje konaènog odgovora na kraju pogaðanja
    if (!remainingInstances.empty()) {
        std::cout << "Konacan odgovor: " << *remainingInstances.begin() << ".\n";
    }
    else {
        std::cout << "Ne mogu da pogodim.\n";
    }
}

void GuessingSystem::guessTry(int num_of_attempts) {
    std::cout << "Pokušaæu da pogodim na šta misliš.\n";

    std::set<std::string> remainingInstances;
    for (const auto& entry : dataset) {
        remainingInstances.insert(entry.first);
    }

    // Postavljanje pitanja u okviru zadatog broja pokušaja
    for (int attempt = 1; attempt <= num_of_attempts && !remainingInstances.empty(); ++attempt) {
        std::cout << "Pitanje " << attempt << ":\n";

        const std::string& question = questions[attempt - 1];
        std::cout << "- " << question;

        std::set<std::string> uniqueAnswers;
        for (const auto& instance : remainingInstances) {
            if (!dataset.at(instance)[attempt - 1].empty()) {
                uniqueAnswers.insert(dataset.at(instance)[attempt - 1]);
            }
        }

        std::cout << "\n";

        if (uniqueAnswers.size() == 1) {
            std::cout << "To je " << *uniqueAnswers.begin() << "!\n";
            return;
        }

        std::string userAnswer;
        std::cin >> userAnswer;

        std::set<std::string> newRemainingInstances;
        for (const auto& instance : remainingInstances) {
            if (dataset.at(instance)[attempt - 1] == userAnswer) {
                newRemainingInstances.insert(instance);
            }
        }
        remainingInstances = newRemainingInstances;

        if (remainingInstances.size() == 1) {
            std::cout << "To je " << *remainingInstances.begin() << "!\n";
            return;
        }
    }

    // Prikazivanje moguæih odgovora nakon svih pitanja
    if (!remainingInstances.empty()) {
        std::cout << "Moguæi odgovori su: ";
        for (const auto& instance : remainingInstances) {
            std::cout << instance << ", ";
        }
        std::cout << "\n";
    }
    else {
        std::cout << "Nisam uspeo da suzim izbor na jednu instancu.\n";
    }

}

void GuessingSystem::displayInfo(const std::string& instanceName) {
    // Traženje instance po delu koji ukljuèuje razmake
    auto instanceIt = std::find_if(dataset.begin(), dataset.end(), [instanceName](const auto& entry) {
        return entry.first.find(instanceName) != std::string::npos;
        });

    if (instanceIt != dataset.end()) {
        const auto& instance = instanceIt->second;

        // Prikazivanje pitanja i odgovora
        std::cout << "Informacije o instanci '" << instanceIt->first << "':\n";
        for (int i = 0; i < questions.size(); ++i) {
            const std::string& question = questions[i];
            const std::string& answer = instance[i];

            std::cout << "- " << question << ": ";

            // Provera da li je odgovor višereèni izraz
            if (answer.find(',') != std::string::npos) {
                // Ako jeste, ispiši svaku reè odvojeno
                std::istringstream answerStream(answer);
                std::string word;
                while (std::getline(answerStream, word, ',')) {
                    std::cout << word << ", ";
                }
            }
            else {
                // Inaèe, ispiši odgovor kao celinu
                std::cout << answer;
            }

            std::cout << "\n";

        }
    }
    else {
        std::cout << "Nema informacija za instancu koja sadrži '" << instanceName << "'.\n";
    }
}

void GuessingSystem::addNewInstance(const std::vector<std::string>& instanceData, const std::string& instanceName, const std::string& filePath) {
    // Otvori fajl za dodavanje na kraj
    std::ofstream outputFile(filePath, std::ios::out | std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Greška prilikom otvaranja fajla za upisivanje.\n";
        return;
    }

    // Upisivanje podataka za novu instancu
    for (int i = 0; i < instanceData.size(); ++i) {
        outputFile << instanceData[i];
        if (i < instanceData.size() - 1) {
            outputFile << ",";  // Dodaj zarez izmeðu podataka, ali ne nakon poslednjeg podatka
        }
    }
    outputFile << "\n";  // Dodaj novi red

    std::cout << "Nova instanca je dodata na kraj fajla.\n";

    // Zatvori fajl
    outputFile.close();
}

void GuessingSystem::deleteInstance(const std::string& instanceName, const std::string& filePath) {
    // Otvori fajl za èitanje i pisanje
    std::fstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Greška prilikom otvaranja fajla za èitanje i pisanje.\n";
        return;
    }

    // Otvori privremeni fajl za pisanje
    std::ofstream tempFile("temp.csv");
    if (!tempFile.is_open()) {
        std::cerr << "Greška prilikom otvaranja privremenog fajla za pisanje.\n";
        file.close();
        return;
    }

    std::string line;
    bool found = false;

    // Proði kroz svaku liniju u originalnom fajlu
    while (std::getline(file, line)) {
        // Ako linija sadrži traženu instancu, preskoèi je
        if (line.find(instanceName) != std::string::npos) {
            found = true;
            continue;
        }

        // Upiši liniju u privremeni fajl
        tempFile << line << "\n";
    }

    // Zatvori oba fajla
    file.close();
    tempFile.close();

    // Ako smo pronašli i obrisali instancu, zameni originalni fajl sa privremenim
    if (found) {
        std::remove(filePath.c_str());
        std::rename("temp.csv", filePath.c_str());
        std::cout << "Instanca '" << instanceName << "' je uspešno obrisana iz fajla.\n";
    }
    else {
        std::cout << "Instanca '" << instanceName << "' nije pronaðena u fajlu.\n";
        std::remove("temp.csv");
    }
}



