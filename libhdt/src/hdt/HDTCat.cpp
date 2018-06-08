#include <HDTVocabulary.hpp>
#include "HDTCat.hpp"

namespace hdt {

HDTCat::HDTCat(HDTSpecification &spec, string baseUri, HDT *hdt1, HDT *hdt2, ProgressListener *listener) : BasicHDT(spec)
{
    try {
        // Make sure that URI starts and ends with <>
        if(baseUri.at(0)!='<')
            baseUri = '<'+baseUri;
        if(baseUri.at(baseUri.length()-1)!='>')
            baseUri.append(">");

        dictionaryCat = new FourSectionDictionaryCat(spec);
        dictionaryCat->cat(hdt1->getDictionary(), hdt2->getDictionary());
        this->dictionary = dictionaryCat;
        triplesCat = new BitmapTriplesCat(spec);
        triplesCat->cat(hdt1->getTriples(), hdt2->getTriples(), dictionaryCat, listener);
        this->triples = triplesCat;
        this->fillHeaderCat(baseUri);
    } catch (std::exception& e) {
        cerr << "Catch exception in HDTCat: " << e.what() << endl;
        deleteComponents();
        throw;
    }

}

void HDTCat::fillHeaderCat(const string& baseUri) {
    string formatNode = "_:format";
    string dictNode = "_:dictionary";
    string triplesNode = "_:triples";
    string statisticsNode = "_:statistics";
    string publicationInfoNode = "_:publicationInformation";

    // BASE
    header->insert(baseUri, HDTVocabulary::RDF_TYPE, HDTVocabulary::HDT_DATASET);

    // VOID
    header->insert(baseUri, HDTVocabulary::RDF_TYPE, HDTVocabulary::VOID_DATASET);
    header->insert(baseUri, HDTVocabulary::VOID_TRIPLES, triples->getNumberOfElements());
    header->insert(baseUri, HDTVocabulary::VOID_PROPERTIES, dictionary->getNpredicates());
    header->insert(baseUri, HDTVocabulary::VOID_DISTINCT_SUBJECTS, dictionary->getNsubjects());
    header->insert(baseUri, HDTVocabulary::VOID_DISTINCT_OBJECTS, dictionary->getNobjects());
    // TODO: Add more VOID Properties. E.g. void:classes

    // Structure
    header->insert(baseUri, HDTVocabulary::HDT_STATISTICAL_INFORMATION,	statisticsNode);
    header->insert(baseUri, HDTVocabulary::HDT_PUBLICATION_INFORMATION,	publicationInfoNode);
    header->insert(baseUri, HDTVocabulary::HDT_FORMAT_INFORMATION, formatNode);
    header->insert(formatNode, HDTVocabulary::HDT_DICTIONARY, dictNode);
    header->insert(formatNode, HDTVocabulary::HDT_TRIPLES, triplesNode);

    // Dictionary
    dictionary->populateHeader(*header, dictNode);

    // Triples
    triples->populateHeader(*header, triplesNode);

    // Sizes
    header->insert(statisticsNode, HDTVocabulary::HDT_SIZE, getDictionary()->size() + getTriples()->size());

    // Current time
    time_t now;
    char date[40];
    time(&now);
    struct tm* today = localtime(&now);
    strftime(date, 40, "%Y-%m-%dT%H:%M:%S%z", today);
    char *tzm = date+strlen(date) - 2;
    memmove(tzm+1, tzm, 3);
    *tzm = ':';
    header->insert(publicationInfoNode, HDTVocabulary::DUBLIN_CORE_ISSUED, date);
}

}
