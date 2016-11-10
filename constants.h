#pragma once

#include <string>
using std::string;

//RELEASE
const string HOME_DIR = "/scratch2/jsybran";

//DEBUG
//const std::string HOME_DIR = "/home/jsybran/Projects/Data";

const std::string MEDLINE_XML_DIR = HOME_DIR + "/medline";
const std::string RESULTS_DIR = HOME_DIR + "/results";
const std::string RES_FILES_DIR = RESULTS_DIR + "/res";
const std::string BACKUP_FILES_DIR = RESULTS_DIR + "/backup";
const std::string CANON_FILE = RESULTS_DIR + "/canon";
const std::string LOG_FILE = HOME_DIR + "/log.txt";


const std::string ABSTRACT_REGEX = "\\</?AbstractText.*?\\>";
const std::string PMID_REGEX = "\\</?PMID.*?\\>";

const std::string KEYWORD_FILE = HOME_DIR + "/keywordslist";
