#include "connection.hpp"

#include <metaSMT/BitBlast.hpp>
#include <metaSMT/backend/Boolector.hpp>
#include <metaSMT/backend/SAT_Clause.hpp>
#include <metaSMT/backend/PicoSAT.hpp>
#include <metaSMT/backend/Z3_Backend.hpp>

bool is_unary(const boost::property_tree::ptree& pt)
{
    return pt.find("operand") != pt.not_found();
}

bool is_binary(const boost::property_tree::ptree& pt)
{
    return pt.find("lhs") != pt.not_found() && pt.find("rhs") != pt.not_found();
}

std::string next_line(socket_ptr socket, boost::asio::streambuf& buffer)
{
    boost::system::error_code error;
    size_t length = read_until(*socket, buffer, '\n', error);
    //if (error == boost::asio::error::eof) break;
    /*else*/ if (error) throw boost::system::system_error(error);

    std::istream is(&buffer);
    std::string s;
    std::getline(is, s);
    s.erase(s.find_last_not_of(" \n\r\t") + 1);

    return s;
}

void new_connection(socket_ptr socket)
{
    std::string solvers = "0 z3\n1 picosat\n2 boolector\n";
    boost::asio::write(*socket, boost::asio::buffer(solvers, solvers.size()));

    std::string ret = "OK\n";

    boost::asio::streambuf buffer;
    std::string s = next_line(socket, buffer);
    int solver;
    try {
        solver = boost::lexical_cast<unsigned>(s);
    } catch (...) {
        solver = -1;
    }

    ConnectionBase* connection;
    switch(solver) {
    case 0:
        connection = new Connection<metaSMT::solver::Z3_Backend>(socket, &buffer);
        break;
    case 1:
        connection = new Connection<metaSMT::BitBlast<metaSMT::SAT_Clause<metaSMT::solver::PicoSAT> > >(socket, &buffer);
        break;
    case 2:
        connection = new Connection<metaSMT::solver::Boolector>(socket, &buffer);
        break;
    default:
        ret = "FAIL unsupported solver\n";
        boost::asio::write(*socket, boost::asio::buffer(ret, ret.size()));
        return;
    }

    boost::asio::write(*socket, boost::asio::buffer(ret, ret.size()));
    connection->start();
}
