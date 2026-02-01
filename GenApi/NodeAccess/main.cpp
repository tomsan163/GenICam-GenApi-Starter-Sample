/*
GenICam GenApi Starter Sample (C++)
* [Description]
A minimal sample for reading and writing Integer and Enumeration nodes using the GenICam GenApi.
* [License]
Copyright (c) 2026 tomsan163. All Rights Reserved.
This software is released under the MIT License.
https://opensource.org/licenses/MIT
* [Notice]
Here are some other examples showing how to use GenApi:
https://booth.pm/ja/items/7780321
*/

#include <stdio.h>
#include <iostream>
#include <GenICam.h>
#include <GenTL.h>
using namespace GenTL;
using namespace GenApi;
using namespace std;

#define GCGetPortInfo(...) /*this is for debug*/

#define GENICAM_CATCH()                                             \
    catch (GenICam::GenericException & ex)                          \
    {                                                               \
        std::cout << "GenericException:" << ex.what() << std::endl; \
        rtn = GC_ERR_ERROR;                                         \
    }                                                               \
    catch (std::exception & ex)                                     \
    {                                                               \
        std::cout << "std::exception:" << ex.what() << std::endl;   \
        rtn = GC_ERR_ERROR;                                         \
    }                                                               \
    catch (...)                                                     \
    {                                                               \
        std::cout << "unknow exception" << std::endl;               \
        rtn = GC_ERR_ERROR;                                         \
    }

class CPort : public GenApi::IPort
{
public:
    CPort() {}
    ~CPort() {}
    void Read(void *pBuffer, int64_t Address, int64_t Length)
    {
        // need to implement ReadFunc(pBuffer, Address, Length) like GCReadPort
    }
    void Write(const void *pBuffer, int64_t Address, int64_t Length)
    {
        // need to implement WriteFunc(pBuffer, Address, Length) like GCWritePort
    }
    EAccessMode GetAccessMode() const
    {
        return RW;
    }
};

int main(int argc, char **argv)
{

    int rtn = 0;
    try
    {
        GenApi::CNodeMapRef cNodeMapRef;
        GenApi::INodeMap *pINodeMap = NULL;
        GenApi::INode *pINode = NULL;

        CPort cPort;
        cNodeMapRef._LoadXMLFromFile("Test.xml");
        cNodeMapRef._Connect(&cPort);

        // If you want to access a Feature (Width, PixelFormat, etc.), you must use an INode.
        /// Note that the destructor of CNodeMapRef will release the GenICam resources, if you use CNodeMapRef
        // get node
        // LoadXml() <-you must call LoadXml function before using CNodeMapRef
        {
            // get node
            /// case 1
            {
                pINode = cNodeMapRef._GetNode("Width");
            }
            /// case 2
            {
                pINodeMap = cNodeMapRef._Ptr;
                pINode = pINodeMap->GetNode("Width");
            }
            // get interface num. This is to check what type the INode is (Integer, Enumeration, etc.).
            int val = pINode->GetPrincipalInterfaceType();

            // pINodeMap
            {
                // Get the number of all nodes
                /// case 1
                {
                    int64_t val = pINodeMap->GetNumNodes();
                    std::cout << val << std::endl;
                }
                /// case 2
                {
                    NodeList_t cNodeList;
                    pINodeMap->GetNodes(cNodeList);
                    std::cout << cNodeList.size() << std::endl;
                }

                // access node
                /// GetNodes() retrieves all nodes containing features that are not <feature> tags.
                {
                    NodeList_t cNodeList;
                    pINodeMap->GetNodes(cNodeList);
                    for (size_t index = 0; index < cNodeList.size(); index++)
                    {
                        pINode = cNodeList.at(index);
                        bool IsFeature = pINode->IsFeature(); // IsFeature() can determine if it is a <feature> .
                        std::cout << pINode->GetName() << std::endl;
                    }
                }
            }
        }

        // Category
        /// The node Root is mandatory, so you should use Root.
        /// Almost all Xml is linked in the order "Root->SubCategories->Feature".
        /// If you don't know the name of the feature, this will help.
        pINode = pINodeMap->GetNode("Root");
        if (pINode->GetPrincipalInterfaceType() == GenApi::intfICategory)
        {
            GenApi::CCategoryPtr cGenicamPtr = pINode;
            GenApi::FeatureList_t cFeatureList;
            cGenicamPtr->GetFeatures(cFeatureList);
            for (size_t index = 0; index < cFeatureList.size(); index++)
            {
                GenApi::INode *pINodeSubCategory = cFeatureList[index]->GetNode();
                std::cout << pINodeSubCategory->GetName() << std::endl;
            }
        }

        // Integer
        pINode = pINodeMap->GetNode("Width");
        if (pINode->GetPrincipalInterfaceType() == GenApi::intfIInteger)
        {
            CIntegerPtr cGenicamPtr = pINode;
            int64_t val = 0;
            // get value
            {
                val = cGenicamPtr->GetValue();
                std::cout << val << std::endl;
            }
            // set value
            {
                cGenicamPtr->SetValue(1);
                // get
                val = cGenicamPtr->GetValue();
                std::cout << val << std::endl;
            }
            // get max
            {
                val = cGenicamPtr->GetMax();
                std::cout << "max:" << val << std::endl;
            }
            // get min
            {
                val = cGenicamPtr->GetMin();
                std::cout << "min:" << val << std::endl;
            }
        }

        // Boolean
        pINode = pINodeMap->GetNode("ReverseX");
        if (pINode->GetPrincipalInterfaceType() == GenApi::intfIBoolean)
        {
            GenApi::CBooleanPtr cGenicamPtr = pINode;
            bool val = false;
            // get value
            {
                val = cGenicamPtr->GetValue();
                std::cout << val << std::endl;
            }
            // set value
            {
                cGenicamPtr->SetValue(true);
                // get
                val = cGenicamPtr->GetValue();
                std::cout << val << std::endl;
            }
        }

        // Commad
        pINode = pINodeMap->GetNode("AcquisitionStart");
        if (pINode->GetPrincipalInterfaceType() == GenApi::intfICommand)
        {
            GenApi::CCommandPtr cGenicamPtr = pINode;
            bool val = false;
            // set value
            {
                cGenicamPtr->Execute();
            }
            // You can see that Execute() was executed. you need poling.
            // Note that if the function's access mode is WO, IsDone() returns immediately.
            {
                for (size_t index = 0; index < 100; index++)
                {
                    if (cGenicamPtr->IsDone())
                    {
                        std::cout << "IsDone:true" << std::endl;
                        break;
                    }
                }
            }
        }

        // Float
        pINode = pINodeMap->GetNode("ExposureTime");
        if (pINode->GetPrincipalInterfaceType() == GenApi::intfIFloat)
        {
            CFloatPtr cGenicamPtr = pINode;
            double val = 0.0;
            // get value
            {
                val = cGenicamPtr->GetValue();
                std::cout << val << std::endl;
            }
            // set value
            {
                cGenicamPtr->SetValue(20000.0);
                // get
                val = cGenicamPtr->GetValue();
                std::cout << val << std::endl;
            }
            // get min/max
            {
                double min = cGenicamPtr->GetMin();
                double max = cGenicamPtr->GetMax();
                std::cout << "min:" << min << ", max:" << max << std::endl;
            }
        }

        // String
        pINode = pINodeMap->GetNode("UserSetDescription");
        if (pINode->GetPrincipalInterfaceType() == GenApi::intfIString)
        {
            GenApi::CStringPtr cGenicamPtr = pINode;
            GenICam::gcstring val = "";
            // get value
            {
                val = cGenicamPtr->GetValue();
                std::cout << val << std::endl;
            }
            // set value
            {
                cGenicamPtr->SetValue("test string");
                // get
                val = cGenicamPtr->GetValue();
                std::cout << val << std::endl;
            }
        }

        // Register
        pINode = pINodeMap->GetNode("GenDCDescriptor");
        if (pINode->GetPrincipalInterfaceType() == GenApi::intfIRegister)
        {
            GenApi::CRegisterPtr cGenicamPtr = pINode;

            // get
            {
                int64_t iSize = cGenicamPtr->GetLength();
                std::vector<uint8_t> cVec = std::vector<uint8_t>(iSize);
                cGenicamPtr->Get(&cVec[0], iSize);
            }
        }

        // Enumeration/EnumEntry
        pINode = pINodeMap->GetNode("PixelFormat");
        if (pINode->GetPrincipalInterfaceType() == GenApi::intfIEnumeration)
        {
            CEnumerationPtr cGenicamPtr = pINode;
            // get value
            {
                // get integer
                {
                    int64_t val = 0;
                    /// case 1
                    {
                        val = cGenicamPtr->GetIntValue();
                        std::cout << val << std::endl;
                    }
                    /// case 2
                    IEnumEntry *pIEnumEntry = cGenicamPtr->GetCurrentEntry();
                    {
                        val = pIEnumEntry->GetValue();
                        std::cout << val << std::endl;
                    }
                }
                // get string
                {
                    GenICam::gcstring val = "";
                    IEnumEntry *pIEnumEntry = cGenicamPtr->GetCurrentEntry();
                    /// recomend GetSymbolic() if you get IEnumEntry's name
                    val = pIEnumEntry->GetSymbolic();
                    std::cout << val << std::endl;
                }
            }
            // set value
            {
                int64_t val = 0;
                /// case 1 : set by integer
                {
                    cGenicamPtr->SetIntValue(17825809);
                    // get
                    val = cGenicamPtr->GetIntValue();
                    std::cout << val << std::endl;
                }
                /// case 2 : set by string
                {
                    IEnumEntry *pIEnumEntry = cGenicamPtr->GetEntryByName("Mono8");
                    cGenicamPtr->SetIntValue(pIEnumEntry->GetValue());
                    std::cout << cGenicamPtr->GetCurrentEntry()->GetSymbolic() << std::endl;
                }
            }
        }
    }
    GENICAM_CATCH();

    std::cout << "press Enter to end\n";
    getchar();
    return 0;
}
