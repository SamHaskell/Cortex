#include <Badger.hpp>

#include "other.hpp"

class Testbed : public Badger::App {
    public:
        Testbed() {

        }
        ~Testbed() {
            
        }
    private:
};

Badger::App *Badger::CreateApp() {
    return new Testbed();
}