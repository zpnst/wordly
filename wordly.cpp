
# include <random>
# include <chrono>
# include <vector>
# include <fstream>
# include <iostream>
# include <unordered_set>

# define WORD_LENGHT 5
# define LETTER_INDEX 3
# define ATTEMPTS_NUMBER 6


std::string blue = "32";
std::string green = "34";


class Letter  {

    std::vector<std::string> current_letter;

    protected:
        Letter(void) = default;
        ~Letter(void) = default;
    
        std::vector<std::string> make_letter(char& letter) {
            current_letter = {"\033[1;", "", "m", std::string(1, letter), "\033[0m"};
            return current_letter;
        }
        void change_letter_color(std::vector<std::string>& letter, std::string& color) {
            letter[1] = color;
        }
        
    public:

        std::string operator[] (unsigned int index) {
            return current_letter[index];
        }
};

class Word : Letter {

    std::vector<std::vector<std::string>> word;

    public:
        Word(std::string word) {
            for (unsigned int iter = 0; iter < WORD_LENGHT; iter++)
                this->word.push_back(make_letter(word[iter]));
        }
        ~Word(void) = default;

        void print_word() {
            std::string result;
            for (unsigned int iter = 0; iter < WORD_LENGHT; iter++)
                for (unsigned int jter = 0; jter < WORD_LENGHT; jter++) 
                    result += word[iter][jter];
            std::cout << result;
        }
        void change_color(unsigned int index, std::string& color) {
            change_letter_color(word[index], color);
        }

        std::vector<std::string> operator[] (unsigned int index) {
            return word[index];
        }

        bool operator== (Word& other) {
            bool flag = true;
            for (unsigned int iter = 0; iter < WORD_LENGHT; iter++) {
                if (this->word[iter][LETTER_INDEX] == other.word[iter][LETTER_INDEX]) flag = flag && true;
                else flag = flag && false;
            }
            return flag;
        }
};


class Game {

    std::unordered_set<std::string> dictionary;
    
    void compare(Word& current, Word& correct) {
        
        std::vector<bool> cashe(WORD_LENGHT, false);
        std::vector<bool> current_cashe(WORD_LENGHT, false);
        
        for (unsigned int it = 0; it < WORD_LENGHT; it++) 
            for (unsigned int jt = 0; jt < WORD_LENGHT; jt++)
                if (current[it][LETTER_INDEX] == correct[jt][LETTER_INDEX] && it == jt && !current_cashe[jt] && !cashe[it]) {
                    current.change_color(it, green);
                    cashe[it] = true; current_cashe[jt] = true;
                }

        for (unsigned int it = 0; it < WORD_LENGHT; it++)
            for (unsigned int jt = 0; jt < WORD_LENGHT; jt++)
                if (current[it][LETTER_INDEX] == correct[jt][LETTER_INDEX] && !current_cashe[jt] && !cashe[it]) {                    
                    current.change_color(it, blue);
                    cashe[it] = true; current_cashe[jt] = true;
                }
    }

    void print_cashe(std::vector<Word>& cashe) {
        for (unsigned int iter = 0; iter < cashe.size(); iter++) {
            cashe[iter].print_word(); std::cout << std::endl;
        }
    }

    std::string word_to_string(Word& some_word) {

        std::string string_word = "";
        for (unsigned int iter = 0; iter < WORD_LENGHT; iter++)
            string_word += some_word[iter][LETTER_INDEX];
        return string_word;
    }

    bool in_dictionary(Word& some_word) {

        std::string string_word = word_to_string(some_word);
        if (dictionary.count(string_word)) return true;
        return false;
    }
    
    std::string get_random_value() {

        std::random_device rand;
        std::mt19937 genration(rand());
        std::uniform_int_distribution<int> distance(0, dictionary.size() - 1);

        auto iter = dictionary.begin();
        std::advance(iter, distance(genration));

        return *iter;
    }

    public:

        Game(void) {

            std::string line;
            std::ifstream ifs("dictionary.txt");
            if (ifs.is_open()) 
                while (std::getline(ifs, line)) 
                    dictionary.insert(line);
                    
            else throw std::runtime_error("dictionary.txt opening error!");
            ifs.close();
        }
        ~Game(void) = default;
        
        void start_wordly() {

            std::vector<Word> cashe;
            unsigned int attemps_count = 0;
            
            std::string random_word = get_random_value();
            Word correct(random_word);
            
            auto start = std::chrono::steady_clock::now();

            std::cout << "Enter the initial word: ";
            while (true) 
            {       
                if (attemps_count == ATTEMPTS_NUMBER) {
                    
                    auto end = std::chrono::steady_clock::now();
                    std::chrono::duration<double> time = end - start;

                    std::cout << "\n\033[1;31mTHE WORD IS NOT GUESSED =(\033[0m\n\033[1;31mYour word was:\033[0m " 
                              << "\033[1;34m" << random_word << "\033[0m" << "\n\n\033[1;32mYour score:\033[0m " << std::endl;

                    std::cout << "\033[1;32mTime:\033[0m " << (unsigned int)time.count() << " seconds" << std::endl;
                    std::cout << "\033[1;32mNumber of attempts:\033[0m " << attemps_count << "/6";
                    break;
                }

                std::string some_word; 
                if (attemps_count != 0) std::cout << "Try to guess: "; 
                std::cin >> some_word; 
                if (attemps_count != 0) std::cout << std::endl;
                
                Word current(some_word);

                if (!in_dictionary(current)) {

                    std::string string_word = word_to_string(current);
                    std::cout << "\033[1;31mWord\033[0m "<< string_word << " \033[1;31mis not in the dictionary! Try again!\n\n\033[0m";
                    print_cashe(cashe);   
                    if (cashe.size() == 0) {
                        std::cout << "Enter the initial word: ";
                    }
                    continue;
                }

                attemps_count++;
                compare(current, correct);
                cashe.push_back(current);
                print_cashe(cashe);   

                if (current == correct) {

                    auto end = std::chrono::steady_clock::now();
                    std::chrono::duration<double> time = end - start;

                    std::cout << "\n\033[1;34mCONGRATULATIONS, THE WORD " << random_word << " IS GUESSED =)\033[0m\n\n\033[1;32mYour score:\033[0m " << std::endl;
                    std::cout << "\033[1;32mTime:\033[0m " << (unsigned int)time.count() << " seconds" << std::endl;
                    std::cout << "\033[1;32mNumber of attempts:\033[0m " << attemps_count << "/6";
                    break;
                }
            }
        }
};

int main(void) {
    
    Game game;
    game.start_wordly();

    return 0;
}