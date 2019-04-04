/*
 * Copyright (C) 2018 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#include <boost/program_options.hpp>
#include <cstdint>
#include <iostream>
#include <LogHard/Backend.h>
#include <LogHard/FileAppender.h>
#include <LogHard/Logger.h>
#include <LogHard/StdAppender.h>
#include <memory>
#include <sharemind/controller/SystemController.h>
#include <sharemind/controller/SystemControllerConfiguration.h>
#include <sharemind/controller/SystemControllerGlobals.h>
#include <sharemind/DebugOnly.h>
#include <sharemind/GlobalDeleter.h>
#include <sharemind/MakeUnique.h>
#include <sstream>
#include <string>

#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <boost/algorithm/string.hpp>

namespace sm = sharemind;

struct ExtraIndentExceptionFormatter {

    template <typename OutStream>
    void operator()(std::size_t const exceptionNumber,
                    std::size_t const totalExceptions,
                    std::exception_ptr e,
                    OutStream out) noexcept
    {
        assert(e);
        out << "    ";
        return LogHard::Logger::StandardFormatter()(
                    exceptionNumber,
                    totalExceptions,
                    std::move(e),
                    std::forward<OutStream>(out));
    }

};
class CSVReader
{
    std::string fileName;
    std::string delimeter;

public:
    CSVReader(std::string filename, std::string delm = ",") :
            fileName(filename), delimeter(delm)
    { }

    // Function to fetch data from a CSV File
    std::vector<std::vector<std::string> > getData();
};

/*
* Parses through csv file line by line and returns the data
* in vector of vector of strings.
*/
std::vector<std::vector<std::string> > CSVReader::getData()
{
    std::ifstream file(fileName);
    std::cout<<"read file: "<<fileName<<std::endl;

    std::vector<std::vector<std::string> > dataList;

    std::string line = "";
    // Iterate through each line and split the content using delimeter
    while (getline(file, line))
    {
        //std::cout<<line<<std::endl;
        std::vector<std::string> vec;
        boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
        dataList.push_back(vec);
    }
    // Close the File
    file.close();
    auto size = dataList.size();
    std::cout<<"Loaded rows from CSV : "<<size<<std::endl;
    return dataList;
}

int main(int argc, char ** argv) {
    std::unique_ptr<sm::SystemControllerConfiguration> config;

    try {
        namespace po = boost::program_options;

        po::options_description desc(
            "incomeClientF\n"
            "Usage: incomeClientF [OPTION]...\n\n"
            "Options");
        desc.add_options()
            ("conf,c", po::value<std::string>(),
                "Set the configuration file.")
            ("help", "Print this help");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return EXIT_SUCCESS;
        }
        if (vm.count("conf")) {
            config = sm::makeUnique<sm::SystemControllerConfiguration>(
                         vm["conf"].as<std::string>());
        } else {
            config = sm::makeUnique<sm::SystemControllerConfiguration>();
        }
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    auto logBackend(std::make_shared<LogHard::Backend>());
    logBackend->addAppender(std::make_shared<LogHard::StdAppender>());
    logBackend->addAppender(
                std::make_shared<LogHard::FileAppender>(
                    "incomeClientF.log",
                    LogHard::FileAppender::OVERWRITE));
    const LogHard::Logger logger(logBackend);

    logger.info() << "This is a stand alone Sharemind application demo";
    logger.info() << "It privately computes the scalar product of the following two vectors";



    try {
        sm::SystemControllerGlobals systemControllerGlobals;
        sm::SystemController c(logger, *config);

        // Creating an object of CSVWriter
        CSVReader reader("/home/sharemind/Sharemind-SDK/standalone-demo/income-server-client/TestShare.csv");

        // Get the data from CSV File
        std::vector<std::vector<std::string> > dataList = reader.getData();

        // Initialize the argument map and set the arguments
        sm::SystemController::ValueMap arguments;

        // Generate column type array for income
        auto age = std::make_shared<std::vector<std::uint16_t>>();
        auto workclass = std::make_shared<std::vector<std::uint16_t>>();

        auto fnlwgt = std::make_shared<std::vector<std::uint64_t>>();
        auto education = std::make_shared<std::vector<std::uint16_t>>();
        auto education_num = std::make_shared<std::vector<std::uint16_t>>();
        auto marital_status = std::make_shared<std::vector<std::uint16_t>>();
        auto occupation = std::make_shared<std::vector<std::uint16_t>>();
        auto relationship = std::make_shared<std::vector<std::uint16_t>>();
        auto race = std::make_shared<std::vector<std::uint16_t>>();
        auto sex = std::make_shared<std::vector<std::uint16_t>>();
        auto capital_gain = std::make_shared<std::vector<std::uint64_t>>();
        auto capital_loss = std::make_shared<std::vector<std::uint64_t>>();
        auto hours_per_week = std::make_shared<std::vector<std::uint16_t>>();
        auto native_country = std::make_shared<std::vector<std::uint16_t>>();
        auto income = std::make_shared<std::vector<std::uint16_t>>();
        std::uint32_t column_index = 0;

        for(std::vector<std::string> vec : dataList)
        {

            // start to insert each row in table
            for(std::string data : vec)
            {
                    //based on column index file the column in a row
                    //std::cout<<data<<"<org-val>"<<boost::lexical_cast<uint64_t>(data)<<", ";
                    if (column_index==0)
                         age->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==1)
                         workclass->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==2)
                         fnlwgt->push_back(boost::lexical_cast<uint64_t>(data));
                    else if (column_index==3)
                         education->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==4)
                         education_num->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==5)
                         marital_status->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==6)
                         occupation->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==7)
                         relationship->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==8)
                         race->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==9)
                         sex->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==10)
                         capital_gain->push_back(boost::lexical_cast<uint64_t>(data));
                    else if (column_index==11)
                         capital_loss->push_back(boost::lexical_cast<uint64_t>(data));
                    else if (column_index==12)
                         hours_per_week->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==13)
                         native_country->push_back(boost::lexical_cast<uint16_t>(data));
                    else if (column_index==14)
                         income->push_back(boost::lexical_cast<uint16_t>(data));
                    else
                         std::cout<<" worng code excuted";
                    //std::cout<<data << " , ";
                    column_index++;
            }
            column_index=0;
            //std::cout<<std::endl;
        }
        arguments["age"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(age, age->data()),
                    sizeof(std::uint16_t) * age->size());
        arguments["workclass"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(workclass, workclass->data()),
                    sizeof(std::uint16_t) * workclass->size());
        arguments["fnlwgt"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int64",
                    std::shared_ptr<std::uint64_t>(workclass, fnlwgt->data()),
                    sizeof(std::uint64_t) * fnlwgt->size());
        arguments["education"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(workclass, education->data()),
                    sizeof(std::uint16_t) * education->size());
        arguments["education_num"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(age, education_num->data()),
                    sizeof(std::uint16_t) * education_num->size());
        arguments["marital_status"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(age, marital_status->data()),
                    sizeof(std::uint16_t) * marital_status->size());

                ////// check here it should be complete
        arguments["occupation"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(occupation, occupation->data()),
                    sizeof(std::uint16_t) * occupation->size());
        arguments["relationship"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(relationship, relationship->data()),
                    sizeof(std::uint16_t) * relationship->size());
        arguments["race"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(race, race->data()),
                    sizeof(std::uint16_t) * race->size());
        arguments["sex"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(sex, sex->data()),
                    sizeof(std::uint16_t) * sex->size());
        arguments["capital_gain"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int64",
                    std::shared_ptr<std::uint64_t>(capital_gain, capital_gain->data()),
                    sizeof(std::uint64_t) * capital_gain->size());
        arguments["capital_loss"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int64",
                    std::shared_ptr<std::uint64_t>(capital_loss, capital_loss->data()),
                    sizeof(std::uint64_t) * capital_loss->size());
        arguments["hours_per_week"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(hours_per_week, hours_per_week->data()),
                    sizeof(std::uint16_t) * hours_per_week->size());
        arguments["native_country"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(native_country, native_country->data()),
                    sizeof(std::uint16_t) * native_country->size());
        arguments["income"] =
                std::make_shared<sm::SystemController::Value>(
                    "pd_shared3p",
                    "int16",
                    std::shared_ptr<std::uint16_t>(income, income->data()),
                    sizeof(std::uint16_t) * income->size());


        // Run code
        logger.info() << "Sending secret shared arguments and running SecreC bytecode on the servers";
        sm::SystemController::ValueMap results = c.runCode("incomeServerF.sb", arguments);

        // Print the result
        sm::SystemController::ValueMap::const_iterator it = results.find("c");
    if (it == results.end()) {
            logger.error() << "Missing 'c' result value.";
            return EXIT_FAILURE;
    }

    try {
            auto c = it->second->getValue<std::int64_t>();
            logger.info() << "The computed income client is: " << c;
        } catch (const sm::SystemController::Value::ParseException & e) {
            logger.error() << "Failed to cast 'c' to appropriate type: " <<
                e.what();
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    } catch (const sm::SystemController::WorkerException & e) {
        logger.fatal() << "Multiple exceptions caught:";
        for (size_t i = 0u; i < e.numWorkers(); i++) {
            if (std::exception_ptr ep = e.nested_ptrs()[i]) {
                logger.fatal() << "  Exception from server " << i << ':';
                try {
                    std::rethrow_exception(std::move(ep));
                } catch (...) {
                    logger.printCurrentException<LogHard::Priority::Fatal>(
                                ExtraIndentExceptionFormatter());
                }
            }
        }
    } catch (const std::exception & e) {
        logger.error() << "Caught exception: " << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
