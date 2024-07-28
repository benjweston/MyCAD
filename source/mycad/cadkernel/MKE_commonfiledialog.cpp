#include "MKE_commonfiledialog.h"

#define STRICT_TYPED_ITEMIDS
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>

const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
    {L"Word Document (*.doc)",       L"*.doc"},
    {L"Web Page (*.htm; *.html)",    L"*.htm;*.html"},
    {L"Text Document (*.txt)",       L"*.txt"},
    {L"All Documents (*.*)",         L"*.*"}
};

// Indices of file types
#define INDEX_WORDDOC 1
#define INDEX_WEBPAGE 2
#define INDEX_TEXTDOC 3

// Controls
#define CONTROL_GROUP           2000
#define CONTROL_RADIOBUTTONLIST 2
#define CONTROL_RADIOBUTTON1    1
#define CONTROL_RADIOBUTTON2    2       // It is OK for this to have the same ID as CONTROL_RADIOBUTTONLIST,
                                        // because it is a child control under CONTROL_RADIOBUTTONLIST

// IDs for the Task Dialog Buttons
#define IDC_BASICFILEOPEN                       100
#define IDC_ADDITEMSTOCUSTOMPLACES              101
#define IDC_ADDCUSTOMCONTROLS                   102
#define IDC_SETDEFAULTVALUESFORPROPERTIES       103
#define IDC_WRITEPROPERTIESUSINGHANDLERS        104
#define IDC_WRITEPROPERTIESWITHOUTUSINGHANDLERS 105

CDialogEventHandler::CDialogEventHandler() : _cRef(1) {}

CDialogEventHandler::~CDialogEventHandler() {}

// IFileDialogEvents methods
// This method gets called when the file-type is changed (combo-box selection changes).
// For sample sake, let's react to this event by changing the properties show.
HRESULT CDialogEventHandler::OnTypeChange(IFileDialog* pfd)
{
    IFileSaveDialog* pfsd;
    HRESULT hResult = pfd->QueryInterface(&pfsd);
    if (SUCCEEDED(hResult))
    {
        UINT uIndex;
        hResult = pfsd->GetFileTypeIndex(&uIndex);   // index of current file-type
        if (SUCCEEDED(hResult))
        {
            IPropertyDescriptionList* pdl = NULL;

            switch (uIndex)
            {
            case INDEX_WORDDOC:
                // When .doc is selected, let's ask for some arbitrary property, say Title.
                hResult = PSGetPropertyDescriptionListFromString(L"prop:System.Title", IID_PPV_ARGS(&pdl));
                if (SUCCEEDED(hResult))
                {
                    // FALSE as second param == do not show default properties.
                    hResult = pfsd->SetCollectedProperties(pdl, FALSE);
                    pdl->Release();
                }
                break;

            case INDEX_WEBPAGE:
                // When .html is selected, let's ask for some other arbitrary property, say Keywords.
                hResult = PSGetPropertyDescriptionListFromString(L"prop:System.Keywords", IID_PPV_ARGS(&pdl));
                if (SUCCEEDED(hResult))
                {
                    // FALSE as second param == do not show default properties.
                    hResult = pfsd->SetCollectedProperties(pdl, FALSE);
                    pdl->Release();
                }
                break;

            case INDEX_TEXTDOC:
                // When .txt is selected, let's ask for some other arbitrary property, say Author.
                hResult = PSGetPropertyDescriptionListFromString(L"prop:System.Author", IID_PPV_ARGS(&pdl));
                if (SUCCEEDED(hResult))
                {
                    // TRUE as second param == show default properties as well, but show Author property first in list.
                    hResult = pfsd->SetCollectedProperties(pdl, TRUE);
                    pdl->Release();
                }
                break;
            }
        }
        pfsd->Release();
    }
    return hResult;
}

// IFileDialogControlEvents
// This method gets called when an dialog control item selection happens (radio-button selection. etc).
// For sample sake, let's react to this event by changing the dialog title.
HRESULT CDialogEventHandler::OnItemSelected(IFileDialogCustomize* pfdc, DWORD dwIDCtl, DWORD dwIDItem)
{
    IFileDialog* pfd = NULL;
    HRESULT hResult = pfdc->QueryInterface(&pfd);
    if (SUCCEEDED(hResult))
    {
        if (dwIDCtl == CONTROL_RADIOBUTTONLIST)
        {
            switch (dwIDItem)
            {
            case CONTROL_RADIOBUTTON1:
                hResult = pfd->SetTitle(L"Longhorn Dialog");
                break;

            case CONTROL_RADIOBUTTON2:
                hResult = pfd->SetTitle(L"Vista Dialog");
                break;
            }
        }
        pfd->Release();
    }
    return hResult;
}

// Instance creation helper
HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void** ppv)
{
    *ppv = NULL;
    CDialogEventHandler* pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
    HRESULT hResult = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hResult))
    {
        hResult = pDialogEventHandler->QueryInterface(riid, ppv);
        pDialogEventHandler->Release();
    }
    return hResult;
}

/* Utility Functions *************************************************************************************************************/

// A helper function that converts UNICODE data to ANSI and writes it to the given file.
// We write in ANSI format to make it easier to open the output file in Notepad.
HRESULT _WriteDataToFile(HANDLE hFile, PCWSTR pszDataIn)
{
    // First figure out our required buffer size.
    DWORD cbData = WideCharToMultiByte(CP_ACP, 0, pszDataIn, -1, NULL, 0, NULL, NULL);
    HRESULT hResult = (cbData == 0) ? HRESULT_FROM_WIN32(::GetLastError()) : S_OK;
    if (SUCCEEDED(hResult))
    {
        // Now allocate a buffer of the required size, and call WideCharToMultiByte again to do the actual conversion.
        char* pszData = new (std::nothrow) CHAR[cbData];
        hResult = pszData ? S_OK : E_OUTOFMEMORY;
        if (SUCCEEDED(hResult))
        {
            hResult = WideCharToMultiByte(CP_ACP, 0, pszDataIn, -1, pszData, cbData, NULL, NULL)
                ? S_OK
                : HRESULT_FROM_WIN32(::GetLastError());
            if (SUCCEEDED(hResult))
            {
                DWORD dwBytesWritten = 0;
                hResult = WriteFile(hFile, pszData, cbData - 1, &dwBytesWritten, NULL)
                    ? S_OK
                    : HRESULT_FROM_WIN32(::GetLastError());
            }
            delete[] pszData;
        }
    }
    return hResult;
}

// Helper function to write property/value into a custom file format.
//
// We are inventing a dummy format here:
// [APPDATA]
// xxxxxx
// [ENDAPPDATA]
// [PROPERTY]foo=bar[ENDPROPERTY]
// [PROPERTY]foo2=bar2[ENDPROPERTY]
HRESULT _WritePropertyToCustomFile(PCWSTR pszFileName, PCWSTR pszPropertyName, PCWSTR pszValue)
{
    HANDLE hFile = CreateFileW(pszFileName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS, // We will write only if the file exists.
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    HRESULT hResult = (hFile == INVALID_HANDLE_VALUE) ? HRESULT_FROM_WIN32(::GetLastError()) : S_OK;
    if (SUCCEEDED(hResult))
    {
        const WCHAR           wszPropertyStartTag[] = L"[PROPERTY]";
        const WCHAR           wszPropertyEndTag[] = L"[ENDPROPERTY]\r\n";
        const DWORD           cchPropertyStartTag = (DWORD)wcslen(wszPropertyStartTag);
        const static DWORD    cchPropertyEndTag = (DWORD)wcslen(wszPropertyEndTag);
        DWORD const cchPropertyLine = cchPropertyStartTag +
            cchPropertyEndTag +
            (DWORD)wcslen(pszPropertyName) +
            (DWORD)wcslen(pszValue) +
            2; // 1 for '=' + 1 for NULL terminator.
        PWSTR pszPropertyLine = new (std::nothrow) WCHAR[cchPropertyLine];
        hResult = pszPropertyLine ? S_OK : E_OUTOFMEMORY;
        if (SUCCEEDED(hResult))
        {
            hResult = StringCchPrintfW(pszPropertyLine,
                cchPropertyLine,
                L"%s%s=%s%s",
                wszPropertyStartTag,
                pszPropertyName,
                pszValue,
                wszPropertyEndTag);
            if (SUCCEEDED(hResult))
            {
                hResult = SetFilePointer(hFile, 0, NULL, FILE_END) ? S_OK : HRESULT_FROM_WIN32(::GetLastError());
                if (SUCCEEDED(hResult))
                {
                    hResult = _WriteDataToFile(hFile, pszPropertyLine);
                }
            }
            delete[] pszPropertyLine;
        }
        CloseHandle(hFile);
    }

    return hResult;
}

// Helper function to write dummy content to a custom file format.
//
// We are inventing a dummy format here:
// [APPDATA]
// xxxxxx
// [ENDAPPDATA]
// [PROPERTY]foo=bar[ENDPROPERTY]
// [PROPERTY]foo2=bar2[ENDPROPERTY]
HRESULT _WriteDataToCustomFile(PCWSTR pszFileName)
{
    HANDLE hFile = CreateFileW(pszFileName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,  // Let's always create this file.
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    HRESULT hResult = (hFile == INVALID_HANDLE_VALUE) ? HRESULT_FROM_WIN32(::GetLastError()) : S_OK;
    if (SUCCEEDED(hResult))
    {
        WCHAR wszDummyContent[] = L"[MYAPPDATA]\r\nThis is an example of how to use the IFileSaveDialog interface.\r\n[ENDMYAPPDATA]\r\n";

        hResult = _WriteDataToFile(hFile, wszDummyContent);
        CloseHandle(hFile);
    }
    return hResult;
}

/* Common File Dialog Snippets ***************************************************************************************************/

// This code snippet demonstrates how to work with the common file dialog interface
HRESULT BasicFileOpen()
{
    // CoCreate the File Open Dialog object.
    IFileDialog* pfd = NULL;
    HRESULT hResult = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hResult))
    {
        // Create an event handling object, and hook it up to the dialog.
        IFileDialogEvents* pfde = NULL;
        hResult = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
        if (SUCCEEDED(hResult))
        {
            // Hook up the event handler.
            DWORD dwCookie;
            hResult = pfd->Advise(pfde, &dwCookie);
            if (SUCCEEDED(hResult))
            {
                // Set the options on the dialog.
                DWORD dwFlags;

                // Before setting, always get the options first in order not to override existing options.
                hResult = pfd->GetOptions(&dwFlags);
                if (SUCCEEDED(hResult))
                {
                    // In this case, get shell items only for file system items.
                    hResult = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
                    if (SUCCEEDED(hResult))
                    {
                        // Set the file types to display only. Notice that, this is a 1-based array.
                        hResult = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
                        if (SUCCEEDED(hResult))
                        {
                            // Set the selected file type index to Word Docs for this example.
                            hResult = pfd->SetFileTypeIndex(INDEX_WORDDOC);
                            if (SUCCEEDED(hResult))
                            {
                                // Set the default extension to be ".doc" file.
                                hResult = pfd->SetDefaultExtension(L"doc");
                                if (SUCCEEDED(hResult))
                                {
                                    // Show the dialog
                                    hResult = pfd->Show(NULL);
                                    if (SUCCEEDED(hResult))
                                    {
                                        // Obtain the result, once the user clicks the 'Open' button.
                                        // The result is an IShellItem object.
                                        IShellItem* psiResult;
                                        hResult = pfd->GetResult(&psiResult);
                                        if (SUCCEEDED(hResult))
                                        {
                                            // We are just going to print out the name of the file for sample sake.
                                            PWSTR pszFilePath = NULL;
                                            hResult = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                                            if (SUCCEEDED(hResult))
                                            {
                                                TaskDialog(NULL,
                                                    NULL,
                                                    L"CommonFileDialogApp",
                                                    pszFilePath,
                                                    NULL,
                                                    TDCBF_OK_BUTTON,
                                                    TD_INFORMATION_ICON,
                                                    NULL);
                                                CoTaskMemFree(pszFilePath);
                                            }
                                            psiResult->Release();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                // Unhook the event handler.
                pfd->Unadvise(dwCookie);
            }
            pfde->Release();
        }
        pfd->Release();
    }
    return hResult;
}

// The Common Places area in the File Dialog is extensible.
// This code snippet demonstrates how to extend the Common Places area.
// Look at CDialogEventHandler::OnItemSelected to see how messages pertaining to the added
// controls can be processed.
HRESULT AddItemsToCommonPlaces()
{
    // CoCreate the File Open Dialog object.
    IFileDialog* pfd = NULL;
    HRESULT hResult = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hResult))
    {
        // Always use known folders instead of hard-coding physical file paths.
        // In this case we are using Public Music KnownFolder.
        IKnownFolderManager* pkfm = NULL;
        hResult = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm));
        if (SUCCEEDED(hResult))
        {
            // Get the known folder.
            IKnownFolder* pKnownFolder = NULL;
            hResult = pkfm->GetFolder(FOLDERID_PublicMusic, &pKnownFolder);
            if (SUCCEEDED(hResult))
            {
                // File Dialog APIs need an IShellItem that represents the location.
                IShellItem* psi = NULL;
                hResult = pKnownFolder->GetShellItem(0, IID_PPV_ARGS(&psi));
                if (SUCCEEDED(hResult))
                {
                    // Add the place to the bottom of default list in Common File Dialog.
                    hResult = pfd->AddPlace(psi, FDAP_BOTTOM);
                    if (SUCCEEDED(hResult))
                    {
                        // Show the File Dialog.
                        hResult = pfd->Show(NULL);
                        if (SUCCEEDED(hResult))
                        {
                            //
                            // You can add your own code here to handle the results.
                            //
                        }
                    }
                    psi->Release();
                }
                pKnownFolder->Release();
            }
            pkfm->Release();
        }
        pfd->Release();
    }
    return hResult;
}

// This code snippet demonstrates how to add custom controls in the Common File Dialog.
HRESULT AddCustomControls()
{
    // CoCreate the File Open Dialog object.
    IFileDialog* pfd = NULL;
    HRESULT hResult = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hResult))
    {
        // Create an event handling object, and hook it up to the dialog.
        IFileDialogEvents* pfde = NULL;
        DWORD               dwCookie = 0;
        hResult = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
        if (SUCCEEDED(hResult))
        {
            // Hook up the event handler.
            hResult = pfd->Advise(pfde, &dwCookie);
            if (SUCCEEDED(hResult))
            {
                // Set up a Customization.
                IFileDialogCustomize* pfdc = NULL;
                hResult = pfd->QueryInterface(IID_PPV_ARGS(&pfdc));
                if (SUCCEEDED(hResult))
                {
                    // Create a Visual Group.
                    hResult = pfdc->StartVisualGroup(CONTROL_GROUP, L"Sample Group");
                    if (SUCCEEDED(hResult))
                    {
                        // Add a radio-button list.
                        hResult = pfdc->AddRadioButtonList(CONTROL_RADIOBUTTONLIST);
                        if (SUCCEEDED(hResult))
                        {
                            // Set the state of the added radio-button list.
                            hResult = pfdc->SetControlState(CONTROL_RADIOBUTTONLIST, CDCS_VISIBLE | CDCS_ENABLED);
                            if (SUCCEEDED(hResult))
                            {
                                // Add individual buttons to the radio-button list.
                                hResult = pfdc->AddControlItem(CONTROL_RADIOBUTTONLIST,
                                    CONTROL_RADIOBUTTON1,
                                    L"Change Title to Longhorn");
                                if (SUCCEEDED(hResult))
                                {
                                    hResult = pfdc->AddControlItem(CONTROL_RADIOBUTTONLIST,
                                        CONTROL_RADIOBUTTON2,
                                        L"Change Title to Vista");
                                    if (SUCCEEDED(hResult))
                                    {
                                        // Set the default selection to option 1.
                                        hResult = pfdc->SetSelectedControlItem(CONTROL_RADIOBUTTONLIST,
                                            CONTROL_RADIOBUTTON1);
                                    }
                                }
                            }
                        }
                        // End the visual group.
                        pfdc->EndVisualGroup();
                    }
                    pfdc->Release();
                }

                if (FAILED(hResult))
                {
                    // Unadvise here in case we encounter failures before we get a chance to show the dialog.
                    pfd->Unadvise(dwCookie);
                }
            }
            pfde->Release();
        }

        if (SUCCEEDED(hResult))
        {
            // Now show the dialog.
            hResult = pfd->Show(NULL);
            if (SUCCEEDED(hResult))
            {
                //
                // You can add your own code here to handle the results.
                //
            }
            // Unhook the event handler.
            pfd->Unadvise(dwCookie);
        }
        pfd->Release();
    }
    return hResult;
}

// This code snippet demonstrates how to add default metadata in the Common File Dialog.
// Look at CDialogEventHandler::OnTypeChange to see to change the order/list of properties
// displayed in the Common File Dialog.
HRESULT SetDefaultValuesForProperties()
{
    // CoCreate the File Open Dialog object.
    IFileSaveDialog* pfsd = NULL;
    HRESULT hResult = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfsd));
    if (SUCCEEDED(hResult))
    {
        // Create an event handling object, and hook it up to the dialog.
        IFileDialogEvents* pfde = NULL;
        DWORD               dwCookie = 0;
        hResult = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
        if (SUCCEEDED(hResult))
        {
            // Hook up the event handler.
            hResult = pfsd->Advise(pfde, &dwCookie);
            if (SUCCEEDED(hResult))
            {
                // Set the file types to display.
                hResult = pfsd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
                if (SUCCEEDED(hResult))
                {
                    hResult = pfsd->SetFileTypeIndex(INDEX_WORDDOC);
                    if (SUCCEEDED(hResult))
                    {
                        hResult = pfsd->SetDefaultExtension(L"doc");
                        if (SUCCEEDED(hResult))
                        {
                            IPropertyStore* pps = NULL;

                            // The InMemory Property Store is a Property Store that is
                            // kept in the memory instead of persisted in a file stream.
                            hResult = PSCreateMemoryPropertyStore(IID_PPV_ARGS(&pps));
                            if (SUCCEEDED(hResult))
                            {
                                PROPVARIANT propvarValue = {};
                                hResult = InitPropVariantFromString(L"SampleKeywordsValue", &propvarValue);
                                if (SUCCEEDED(hResult))
                                {
                                    // Set the value to the property store of the item.
                                    hResult = pps->SetValue(PKEY_Keywords, propvarValue);
                                    if (SUCCEEDED(hResult))
                                    {
                                        // Commit does the actual writing back to the in memory store.
                                        hResult = pps->Commit();
                                        if (SUCCEEDED(hResult))
                                        {
                                            // Hand these properties to the File Dialog.
                                            hResult = pfsd->SetCollectedProperties(NULL, TRUE);
                                            if (SUCCEEDED(hResult))
                                            {
                                                hResult = pfsd->SetProperties(pps);
                                            }
                                        }
                                    }
                                    PropVariantClear(&propvarValue);
                                }
                                pps->Release();
                            }
                        }
                    }
                }

                if (FAILED(hResult))
                {
                    // Unadvise here in case we encounter failures before we get a chance to show the dialog.
                    pfsd->Unadvise(dwCookie);
                }
            }
            pfde->Release();
        }

        if (SUCCEEDED(hResult))
        {
            // Now show the dialog.
            hResult = pfsd->Show(NULL);
            if (SUCCEEDED(hResult))
            {
                //
                // You can add your own code here to handle the results.
                //
            }
            // Unhook the event handler.
            pfsd->Unadvise(dwCookie);
        }
        pfsd->Release();
    }
    return hResult;
}

// The following code snippet demonstrates two things:
// 1.  How to write properties using property handlers.
// 2.  Replicating properties in the "Save As" scenario where the user choses to save an existing file
//     with a different name.  We need to make sure we replicate not just the data,
//     but also the properties of the original file.
HRESULT WritePropertiesUsingHandlers()
{
    // CoCreate the File Open Dialog object.
    IFileSaveDialog* pfsd;
    HRESULT hResult = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfsd));
    if (SUCCEEDED(hResult))
    {
        WCHAR szFullPathToTestFile[MAX_PATH] = {};

        // For this exercise, let's just support only one file type to make things simpler.
        // Also, let's use the jpg format for sample purpose because the Windows ships with
        // property handlers for jpg files.
        const COMDLG_FILTERSPEC rgSaveTypes[] = { {L"Photo Document (*.jpg)", L"*.jpg"} };

        // Set the file types to display.
        hResult = pfsd->SetFileTypes(ARRAYSIZE(rgSaveTypes), rgSaveTypes);
        if (SUCCEEDED(hResult))
        {
            hResult = pfsd->SetFileTypeIndex(0);
            if (SUCCEEDED(hResult))
            {
                // Set default file extension.
                hResult = pfsd->SetDefaultExtension(L"jpg");
                if (SUCCEEDED(hResult))
                {
                    // Ensure the dialog only returns items that can be represented by file system paths.
                    DWORD dwFlags;
                    hResult = pfsd->GetOptions(&dwFlags);
                    if (SUCCEEDED(hResult))
                    {
                        hResult = pfsd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);

                        // Let's first get the current property set of the file we are replicating
                        // and give it to the file dialog object.
                        //
                        // For simplicity sake, let's just get the property set from a pre-existing jpg file (in the Pictures folder).
                        // In the real-world, you would actually add code to get the property set of the file
                        // that is currently open and is being replicated.
                        if (SUCCEEDED(hResult))
                        {
                            PWSTR pszPicturesFolderPath;
                            hResult = SHGetKnownFolderPath(FOLDERID_SamplePictures, 0, NULL, &pszPicturesFolderPath);
                            if (SUCCEEDED(hResult))
                            {
                                hResult = PathCombineW(szFullPathToTestFile, pszPicturesFolderPath, L"Flower.jpg") ? S_OK : E_FAIL;
                                if (SUCCEEDED(hResult))
                                {
                                    IPropertyStore* pps;
                                    hResult = SHGetPropertyStoreFromParsingName(szFullPathToTestFile, NULL, GPS_DEFAULT, IID_PPV_ARGS(&pps));
                                    if (FAILED(hResult))
                                    {
                                        // Flower.jpg is probably not in the Pictures folder.
                                        TaskDialog(NULL, NULL, L"CommonFileDialogApp", L"Create Flower.jpg in the Pictures folder and try again.",
                                            NULL, TDCBF_OK_BUTTON, TD_ERROR_ICON, NULL);
                                    }
                                    else
                                    {
                                        // Call SetProperties on the file dialog object for getting back later.
                                        pfsd->SetCollectedProperties(NULL, TRUE);
                                        pfsd->SetProperties(pps);
                                        pps->Release();
                                    }
                                }
                                CoTaskMemFree(pszPicturesFolderPath);
                            }
                        }
                    }
                }
            }
        }

        if (SUCCEEDED(hResult))
        {
            hResult = pfsd->Show(NULL);
            if (SUCCEEDED(hResult))
            {
                IShellItem* psiResult;
                hResult = pfsd->GetResult(&psiResult);
                if (SUCCEEDED(hResult))
                {
                    PWSTR pszNewFileName;
                    hResult = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszNewFileName);
                    if (SUCCEEDED(hResult))
                    {
                        // This is where you add code to write data to your file.
                        // For simplicity, let's just copy a pre-existing dummy jpg file.
                        //
                        // In the real-world, you would actually add code to replicate the data of
                        // file that is currently open.
                        hResult = CopyFileW(szFullPathToTestFile, pszNewFileName, FALSE) ? S_OK : HRESULT_FROM_WIN32(::GetLastError());
                        if (SUCCEEDED(hResult))
                        {
                            // Now apply the properties.
                            //
                            // Get the property store first by calling GetPropertyStore and pass it on to ApplyProperties.
                            // This will make the registered propety handler for the specified file type (jpg)
                            // do all the work of writing the properties for you.
                            //
                            // Property handlers for the specified file type should be registered for this
                            // to work.
                            IPropertyStore* pps;

                            // When we call GetProperties, we get back all the properties that we originally set
                            // (in our call to SetProperties above) plus the ones user modified in the file dialog.
                            hResult = pfsd->GetProperties(&pps);
                            if (SUCCEEDED(hResult))
                            {
                                // Now apply the properties making use of the registered property handler for the file type.
                                //
                                // hWnd is used as parent for any error dialogs that might popup when writing properties.
                                // Pass NULL for IFileOperationProgressSink as we don't want to register any callback for progress notifications.
                                hResult = pfsd->ApplyProperties(psiResult, pps, NULL, NULL);
                                pps->Release();
                            }
                        }
                        CoTaskMemFree(pszNewFileName);
                    }
                    psiResult->Release();
                }
            }
        }
        pfsd->Release();
    }
    return hResult;
}

// This code snippet demonstrates how to write properties without using property handlers.
HRESULT WritePropertiesWithoutUsingHandlers()
{
    // CoCreate the File Open Dialog object.
    IFileSaveDialog* pfsd;
    HRESULT hResult = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfsd));
    if (SUCCEEDED(hResult))
    {
        // For this exercise, let's use a custom file type.
        const COMDLG_FILTERSPEC rgSaveTypes[] = { {L"MyApp Document (*.myApp)", L"*.myApp"} };

        // Set the file types to display.
        hResult = pfsd->SetFileTypes(ARRAYSIZE(rgSaveTypes), rgSaveTypes);
        if (SUCCEEDED(hResult))
        {
            hResult = pfsd->SetFileTypeIndex(0);
            if (SUCCEEDED(hResult))
            {
                // Set default file extension.
                hResult = pfsd->SetDefaultExtension(L"myApp");
                if (SUCCEEDED(hResult))
                {
                    // Ensure the dialog only returns items that can be represented by file system paths.
                    DWORD dwFlags;
                    hResult = pfsd->GetOptions(&dwFlags);
                    if (SUCCEEDED(hResult))
                    {
                        hResult = pfsd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
                        if (SUCCEEDED(hResult))
                        {
                            // Set the properties you want the FileSave dialog to collect from the user.
                            IPropertyDescriptionList* pdl;
                            hResult = PSGetPropertyDescriptionListFromString(L"prop:System.Keywords", IID_PPV_ARGS(&pdl));
                            if (SUCCEEDED(hResult))
                            {
                                // TRUE as second param == show default properties as well, but show Keyword first.
                                hResult = pfsd->SetCollectedProperties(pdl, TRUE);
                                pdl->Release();
                            }
                        }
                    }
                }
            }
        }

        if (SUCCEEDED(hResult))
        {
            // Now show the dialog.
            hResult = pfsd->Show(NULL);
            if (SUCCEEDED(hResult))
            {
                IShellItem* psiResult;
                hResult = pfsd->GetResult(&psiResult);
                if (SUCCEEDED(hResult))
                {
                    // Get the path to the file.
                    PWSTR pszNewFileName;
                    hResult = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszNewFileName);
                    if (SUCCEEDED(hResult))
                    {
                        // Write data to the file.
                        hResult = _WriteDataToCustomFile(pszNewFileName);
                        if (SUCCEEDED(hResult))
                        {
                            // Now get the property store and write each individual property to the file.
                            IPropertyStore* pps;
                            hResult = pfsd->GetProperties(&pps);
                            if (SUCCEEDED(hResult))
                            {
                                DWORD cProps = 0;
                                hResult = pps->GetCount(&cProps);

                                // Loop over property set and write each property/value pair to the file.
                                for (DWORD i = 0; i < cProps && SUCCEEDED(hResult); i++)
                                {
                                    PROPERTYKEY key;
                                    hResult = pps->GetAt(i, &key);
                                    if (SUCCEEDED(hResult))
                                    {
                                        PWSTR pszPropertyName;
                                        hResult = PSGetNameFromPropertyKey(key, &pszPropertyName);
                                        if (SUCCEEDED(hResult))
                                        {
                                            // Get the value of the property.
                                            PROPVARIANT propvarValue;
                                            PropVariantInit(&propvarValue);
                                            hResult = pps->GetValue(key, &propvarValue);
                                            if (SUCCEEDED(hResult))
                                            {
                                                WCHAR wszValue[MAX_PATH];

                                                // Always use property system APIs to do the conversion for you.
                                                hResult = PropVariantToString(propvarValue, wszValue, ARRAYSIZE(wszValue));
                                                if (SUCCEEDED(hResult))
                                                {
                                                    // Write the property to the file.
                                                    hResult = _WritePropertyToCustomFile(pszNewFileName, pszPropertyName, wszValue);
                                                }
                                            }
                                            PropVariantClear(&propvarValue);
                                            CoTaskMemFree(pszPropertyName);
                                        }
                                    }
                                }
                                pps->Release();
                            }
                        }
                        CoTaskMemFree(pszNewFileName);
                    }
                    psiResult->Release();
                }
            }
        }
        pfsd->Release();
    }
    return hResult;
}

// Application entry point
/*int APIENTRY wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    HRESULT hResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hResult))
    {
        TASKDIALOGCONFIG taskDialogParams = { sizeof(taskDialogParams) };
        taskDialogParams.dwFlags = TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION;

        TASKDIALOG_BUTTON const buttons[] =
        {
            { IDC_BASICFILEOPEN,                       L"Basic File Open" },
            { IDC_ADDITEMSTOCUSTOMPLACES,              L"Add Items to Common Places" },
            { IDC_ADDCUSTOMCONTROLS,                   L"Add Custom Controls" },
            { IDC_SETDEFAULTVALUESFORPROPERTIES,       L"Change Property Order" },
            { IDC_WRITEPROPERTIESUSINGHANDLERS,        L"Write Properties Using Handlers" },
            { IDC_WRITEPROPERTIESWITHOUTUSINGHANDLERS, L"Write Properties without Using Handlers" },
        };

        taskDialogParams.pButtons = buttons;
        taskDialogParams.cButtons = ARRAYSIZE(buttons);
        taskDialogParams.pszMainInstruction = L"Pick the file dialog sample you want to try";
        taskDialogParams.pszWindowTitle = L"Common File Dialog";

        while (SUCCEEDED(hResult))
        {
            int selectedId;
            hResult = TaskDialogIndirect(&taskDialogParams, &selectedId, NULL, NULL);
            if (SUCCEEDED(hResult))
            {
                if (selectedId == IDCANCEL)
                {
                    break;
                }
                else if (selectedId == IDC_BASICFILEOPEN)
                {
                    BasicFileOpen();
                }
                else if (selectedId == IDC_ADDITEMSTOCUSTOMPLACES)
                {
                    AddItemsToCommonPlaces();
                }
                else if (selectedId == IDC_ADDCUSTOMCONTROLS)
                {
                    AddCustomControls();
                }
                else if (selectedId == IDC_SETDEFAULTVALUESFORPROPERTIES)
                {
                    SetDefaultValuesForProperties();
                }
                else if (selectedId == IDC_WRITEPROPERTIESUSINGHANDLERS)
                {
                    WritePropertiesUsingHandlers();
                }
                else if (selectedId == IDC_WRITEPROPERTIESWITHOUTUSINGHANDLERS)
                {
                    WritePropertiesWithoutUsingHandlers();
                }
            }
        }
        CoUninitialize();
    }
    return 0;
}*/
