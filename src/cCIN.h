class cCIN
{
public:
static std::string myString;
void operator()()
{
    std::cout << "type something: "; 
    std::cin >> myString;
}
};

std::string cCIN::myString;
cCIN theCIN;
