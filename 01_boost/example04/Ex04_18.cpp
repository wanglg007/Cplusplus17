//Assigning a list of values to a vector
#include <string>
#include <vector>
#include <boost/assign.hpp>
#include <cassert>

using namespace boost::assign;

/**
 * Boost.Assign is one of those nifty little libraries in Boost, which you get into the habit of using at the smallest opportunity.
 * @return
 */
int main() {
    std::vector<std::string> greetings;
    greetings += "Good morning", "Buenos dias", "Bongiorno";
    greetings += "Boker tov", "Guten Morgen", "Bonjour";

    assert(greetings.size() == 6);
}

