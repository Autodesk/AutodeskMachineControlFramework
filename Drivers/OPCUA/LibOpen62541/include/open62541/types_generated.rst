NamingRuleType
^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_NAMINGRULETYPE_MANDATORY = 1,
        UA_NAMINGRULETYPE_OPTIONAL = 2,
        UA_NAMINGRULETYPE_CONSTRAINT = 3
    } UA_NamingRuleType;

Enumeration
^^^^^^^^^^^

.. code-block:: c

    typedef enum {
    } UA_Enumeration;

ImageBMP
^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_ImageBMP;

ImageGIF
^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_ImageGIF;

ImageJPG
^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_ImageJPG;

ImagePNG
^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_ImagePNG;

AudioDataType
^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_AudioDataType;

UriString
^^^^^^^^^

.. code-block:: c

    typedef UA_String UA_UriString;

BitFieldMaskDataType
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt64 UA_BitFieldMaskDataType;

SemanticVersionString
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_String UA_SemanticVersionString;

KeyValuePair
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_QualifiedName key;
        UA_Variant value;
    } UA_KeyValuePair;

AdditionalParametersType
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t parametersSize;
        UA_KeyValuePair *parameters;
    } UA_AdditionalParametersType;

EphemeralKeyType
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ByteString publicKey;
        UA_ByteString signature;
    } UA_EphemeralKeyType;

RationalNumber
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Int32 numerator;
        UA_UInt32 denominator;
    } UA_RationalNumber;

ThreeDVector
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Double x;
        UA_Double y;
        UA_Double z;
    } UA_ThreeDVector;

ThreeDCartesianCoordinates
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Double x;
        UA_Double y;
        UA_Double z;
    } UA_ThreeDCartesianCoordinates;

ThreeDOrientation
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Double a;
        UA_Double b;
        UA_Double c;
    } UA_ThreeDOrientation;

ThreeDFrame
^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ThreeDCartesianCoordinates cartesianCoordinates;
        UA_ThreeDOrientation orientation;
    } UA_ThreeDFrame;

OpenFileMode
^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_OPENFILEMODE_READ = 1,
        UA_OPENFILEMODE_WRITE = 2,
        UA_OPENFILEMODE_ERASEEXISTING = 4,
        UA_OPENFILEMODE_APPEND = 8
    } UA_OpenFileMode;

IdentityCriteriaType
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_IDENTITYCRITERIATYPE_USERNAME = 1,
        UA_IDENTITYCRITERIATYPE_THUMBPRINT = 2,
        UA_IDENTITYCRITERIATYPE_ROLE = 3,
        UA_IDENTITYCRITERIATYPE_GROUPID = 4,
        UA_IDENTITYCRITERIATYPE_ANONYMOUS = 5,
        UA_IDENTITYCRITERIATYPE_AUTHENTICATEDUSER = 6,
        UA_IDENTITYCRITERIATYPE_APPLICATION = 7,
        UA_IDENTITYCRITERIATYPE_X509SUBJECT = 8
    } UA_IdentityCriteriaType;

IdentityMappingRuleType
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_IdentityCriteriaType criteriaType;
        UA_String criteria;
    } UA_IdentityMappingRuleType;

CurrencyUnitType
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Int16 numericCode;
        UA_SByte exponent;
        UA_String alphabeticCode;
        UA_LocalizedText currency;
    } UA_CurrencyUnitType;

TrustListMasks
^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_TRUSTLISTMASKS_NONE = 0,
        UA_TRUSTLISTMASKS_TRUSTEDCERTIFICATES = 1,
        UA_TRUSTLISTMASKS_TRUSTEDCRLS = 2,
        UA_TRUSTLISTMASKS_ISSUERCERTIFICATES = 4,
        UA_TRUSTLISTMASKS_ISSUERCRLS = 8,
        UA_TRUSTLISTMASKS_ALL = 15
    } UA_TrustListMasks;

TrustListDataType
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedLists;
        size_t trustedCertificatesSize;
        UA_ByteString *trustedCertificates;
        size_t trustedCrlsSize;
        UA_ByteString *trustedCrls;
        size_t issuerCertificatesSize;
        UA_ByteString *issuerCertificates;
        size_t issuerCrlsSize;
        UA_ByteString *issuerCrls;
    } UA_TrustListDataType;

DecimalDataType
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Int16 scale;
        UA_ByteString value;
    } UA_DecimalDataType;

DataTypeDescription
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId dataTypeId;
        UA_QualifiedName name;
    } UA_DataTypeDescription;

SimpleTypeDescription
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId dataTypeId;
        UA_QualifiedName name;
        UA_NodeId baseDataType;
        UA_Byte builtInType;
    } UA_SimpleTypeDescription;

PortableQualifiedName
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String namespaceUri;
        UA_String name;
    } UA_PortableQualifiedName;

PortableNodeId
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String namespaceUri;
        UA_NodeId identifier;
    } UA_PortableNodeId;

UnsignedRationalNumber
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 numerator;
        UA_UInt32 denominator;
    } UA_UnsignedRationalNumber;

PubSubState
^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_PUBSUBSTATE_DISABLED = 0,
        UA_PUBSUBSTATE_PAUSED = 1,
        UA_PUBSUBSTATE_OPERATIONAL = 2,
        UA_PUBSUBSTATE_ERROR = 3,
        UA_PUBSUBSTATE_PREOPERATIONAL = 4
    } UA_PubSubState;

DataSetFieldFlags
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt16 UA_DataSetFieldFlags;
    
    #define UA_DATASETFIELDFLAGS_NONE 0
    #define UA_DATASETFIELDFLAGS_PROMOTEDFIELD 1

ConfigurationVersionDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 majorVersion;
        UA_UInt32 minorVersion;
    } UA_ConfigurationVersionDataType;

PublishedVariableDataType
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId publishedVariable;
        UA_UInt32 attributeId;
        UA_Double samplingIntervalHint;
        UA_UInt32 deadbandType;
        UA_Double deadbandValue;
        UA_String indexRange;
        UA_Variant substituteValue;
        size_t metaDataPropertiesSize;
        UA_QualifiedName *metaDataProperties;
    } UA_PublishedVariableDataType;

PublishedDataItemsDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t publishedDataSize;
        UA_PublishedVariableDataType *publishedData;
    } UA_PublishedDataItemsDataType;

PublishedDataSetCustomSourceDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Boolean cyclicDataSet;
    } UA_PublishedDataSetCustomSourceDataType;

DataSetFieldContentMask
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_DataSetFieldContentMask;
    
    #define UA_DATASETFIELDCONTENTMASK_NONE 0
    #define UA_DATASETFIELDCONTENTMASK_STATUSCODE 1
    #define UA_DATASETFIELDCONTENTMASK_SOURCETIMESTAMP 2
    #define UA_DATASETFIELDCONTENTMASK_SERVERTIMESTAMP 4
    #define UA_DATASETFIELDCONTENTMASK_SOURCEPICOSECONDS 8
    #define UA_DATASETFIELDCONTENTMASK_SERVERPICOSECONDS 16
    #define UA_DATASETFIELDCONTENTMASK_RAWDATA 32

DataSetWriterDataType
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        UA_Boolean enabled;
        UA_UInt16 dataSetWriterId;
        UA_DataSetFieldContentMask dataSetFieldContentMask;
        UA_UInt32 keyFrameCount;
        UA_String dataSetName;
        size_t dataSetWriterPropertiesSize;
        UA_KeyValuePair *dataSetWriterProperties;
        UA_ExtensionObject transportSettings;
        UA_ExtensionObject messageSettings;
    } UA_DataSetWriterDataType;

NetworkAddressDataType
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String networkInterface;
    } UA_NetworkAddressDataType;

NetworkAddressUrlDataType
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String networkInterface;
        UA_String url;
    } UA_NetworkAddressUrlDataType;

OverrideValueHandling
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_OVERRIDEVALUEHANDLING_DISABLED = 0,
        UA_OVERRIDEVALUEHANDLING_LASTUSABLEVALUE = 1,
        UA_OVERRIDEVALUEHANDLING_OVERRIDEVALUE = 2
    } UA_OverrideValueHandling;

StandaloneSubscribedDataSetRefDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String dataSetName;
    } UA_StandaloneSubscribedDataSetRefDataType;

DataSetOrderingType
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_DATASETORDERINGTYPE_UNDEFINED = 0,
        UA_DATASETORDERINGTYPE_ASCENDINGWRITERID = 1,
        UA_DATASETORDERINGTYPE_ASCENDINGWRITERIDSINGLE = 2
    } UA_DataSetOrderingType;

UadpNetworkMessageContentMask
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_UadpNetworkMessageContentMask;
    
    #define UA_UADPNETWORKMESSAGECONTENTMASK_NONE 0
    #define UA_UADPNETWORKMESSAGECONTENTMASK_PUBLISHERID 1
    #define UA_UADPNETWORKMESSAGECONTENTMASK_GROUPHEADER 2
    #define UA_UADPNETWORKMESSAGECONTENTMASK_WRITERGROUPID 4
    #define UA_UADPNETWORKMESSAGECONTENTMASK_GROUPVERSION 8
    #define UA_UADPNETWORKMESSAGECONTENTMASK_NETWORKMESSAGENUMBER 16
    #define UA_UADPNETWORKMESSAGECONTENTMASK_SEQUENCENUMBER 32
    #define UA_UADPNETWORKMESSAGECONTENTMASK_PAYLOADHEADER 64
    #define UA_UADPNETWORKMESSAGECONTENTMASK_TIMESTAMP 128
    #define UA_UADPNETWORKMESSAGECONTENTMASK_PICOSECONDS 256
    #define UA_UADPNETWORKMESSAGECONTENTMASK_DATASETCLASSID 512
    #define UA_UADPNETWORKMESSAGECONTENTMASK_PROMOTEDFIELDS 1024

UadpWriterGroupMessageDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 groupVersion;
        UA_DataSetOrderingType dataSetOrdering;
        UA_UadpNetworkMessageContentMask networkMessageContentMask;
        UA_Double samplingOffset;
        size_t publishingOffsetSize;
        UA_Double *publishingOffset;
    } UA_UadpWriterGroupMessageDataType;

UadpDataSetMessageContentMask
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_UadpDataSetMessageContentMask;
    
    #define UA_UADPDATASETMESSAGECONTENTMASK_NONE 0
    #define UA_UADPDATASETMESSAGECONTENTMASK_TIMESTAMP 1
    #define UA_UADPDATASETMESSAGECONTENTMASK_PICOSECONDS 2
    #define UA_UADPDATASETMESSAGECONTENTMASK_STATUS 4
    #define UA_UADPDATASETMESSAGECONTENTMASK_MAJORVERSION 8
    #define UA_UADPDATASETMESSAGECONTENTMASK_MINORVERSION 16
    #define UA_UADPDATASETMESSAGECONTENTMASK_SEQUENCENUMBER 32

UadpDataSetWriterMessageDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UadpDataSetMessageContentMask dataSetMessageContentMask;
        UA_UInt16 configuredSize;
        UA_UInt16 networkMessageNumber;
        UA_UInt16 dataSetOffset;
    } UA_UadpDataSetWriterMessageDataType;

UadpDataSetReaderMessageDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 groupVersion;
        UA_UInt16 networkMessageNumber;
        UA_UInt16 dataSetOffset;
        UA_Guid dataSetClassId;
        UA_UadpNetworkMessageContentMask networkMessageContentMask;
        UA_UadpDataSetMessageContentMask dataSetMessageContentMask;
        UA_Double publishingInterval;
        UA_Double receiveOffset;
        UA_Double processingOffset;
    } UA_UadpDataSetReaderMessageDataType;

JsonNetworkMessageContentMask
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_JsonNetworkMessageContentMask;
    
    #define UA_JSONNETWORKMESSAGECONTENTMASK_NONE 0
    #define UA_JSONNETWORKMESSAGECONTENTMASK_NETWORKMESSAGEHEADER 1
    #define UA_JSONNETWORKMESSAGECONTENTMASK_DATASETMESSAGEHEADER 2
    #define UA_JSONNETWORKMESSAGECONTENTMASK_SINGLEDATASETMESSAGE 4
    #define UA_JSONNETWORKMESSAGECONTENTMASK_PUBLISHERID 8
    #define UA_JSONNETWORKMESSAGECONTENTMASK_DATASETCLASSID 16
    #define UA_JSONNETWORKMESSAGECONTENTMASK_REPLYTO 32

JsonWriterGroupMessageDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_JsonNetworkMessageContentMask networkMessageContentMask;
    } UA_JsonWriterGroupMessageDataType;

JsonDataSetMessageContentMask
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_JsonDataSetMessageContentMask;
    
    #define UA_JSONDATASETMESSAGECONTENTMASK_NONE 0
    #define UA_JSONDATASETMESSAGECONTENTMASK_DATASETWRITERID 1
    #define UA_JSONDATASETMESSAGECONTENTMASK_METADATAVERSION 2
    #define UA_JSONDATASETMESSAGECONTENTMASK_SEQUENCENUMBER 4
    #define UA_JSONDATASETMESSAGECONTENTMASK_TIMESTAMP 8
    #define UA_JSONDATASETMESSAGECONTENTMASK_STATUS 16
    #define UA_JSONDATASETMESSAGECONTENTMASK_MESSAGETYPE 32
    #define UA_JSONDATASETMESSAGECONTENTMASK_DATASETWRITERNAME 64
    #define UA_JSONDATASETMESSAGECONTENTMASK_REVERSIBLEFIELDENCODING 128

JsonDataSetWriterMessageDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_JsonDataSetMessageContentMask dataSetMessageContentMask;
    } UA_JsonDataSetWriterMessageDataType;

JsonDataSetReaderMessageDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_JsonNetworkMessageContentMask networkMessageContentMask;
        UA_JsonDataSetMessageContentMask dataSetMessageContentMask;
    } UA_JsonDataSetReaderMessageDataType;

TransmitQosPriorityDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String priorityLabel;
    } UA_TransmitQosPriorityDataType;

ReceiveQosPriorityDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String priorityLabel;
    } UA_ReceiveQosPriorityDataType;

DatagramConnectionTransportDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ExtensionObject discoveryAddress;
    } UA_DatagramConnectionTransportDataType;

DatagramConnectionTransport2DataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ExtensionObject discoveryAddress;
        UA_UInt32 discoveryAnnounceRate;
        UA_UInt32 discoveryMaxMessageSize;
        UA_String qosCategory;
        size_t datagramQosSize;
        UA_ExtensionObject *datagramQos;
    } UA_DatagramConnectionTransport2DataType;

DatagramWriterGroupTransportDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Byte messageRepeatCount;
        UA_Double messageRepeatDelay;
    } UA_DatagramWriterGroupTransportDataType;

DatagramWriterGroupTransport2DataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Byte messageRepeatCount;
        UA_Double messageRepeatDelay;
        UA_ExtensionObject address;
        UA_String qosCategory;
        size_t datagramQosSize;
        UA_ExtensionObject *datagramQos;
        UA_UInt32 discoveryAnnounceRate;
        UA_String topic;
    } UA_DatagramWriterGroupTransport2DataType;

DatagramDataSetReaderTransportDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ExtensionObject address;
        UA_String qosCategory;
        size_t datagramQosSize;
        UA_ExtensionObject *datagramQos;
        UA_String topic;
    } UA_DatagramDataSetReaderTransportDataType;

BrokerConnectionTransportDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String resourceUri;
        UA_String authenticationProfileUri;
    } UA_BrokerConnectionTransportDataType;

BrokerTransportQualityOfService
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_BROKERTRANSPORTQUALITYOFSERVICE_NOTSPECIFIED = 0,
        UA_BROKERTRANSPORTQUALITYOFSERVICE_BESTEFFORT = 1,
        UA_BROKERTRANSPORTQUALITYOFSERVICE_ATLEASTONCE = 2,
        UA_BROKERTRANSPORTQUALITYOFSERVICE_ATMOSTONCE = 3,
        UA_BROKERTRANSPORTQUALITYOFSERVICE_EXACTLYONCE = 4
    } UA_BrokerTransportQualityOfService;

BrokerWriterGroupTransportDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String queueName;
        UA_String resourceUri;
        UA_String authenticationProfileUri;
        UA_BrokerTransportQualityOfService requestedDeliveryGuarantee;
    } UA_BrokerWriterGroupTransportDataType;

BrokerDataSetWriterTransportDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String queueName;
        UA_String resourceUri;
        UA_String authenticationProfileUri;
        UA_BrokerTransportQualityOfService requestedDeliveryGuarantee;
        UA_String metaDataQueueName;
        UA_Double metaDataUpdateTime;
    } UA_BrokerDataSetWriterTransportDataType;

BrokerDataSetReaderTransportDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String queueName;
        UA_String resourceUri;
        UA_String authenticationProfileUri;
        UA_BrokerTransportQualityOfService requestedDeliveryGuarantee;
        UA_String metaDataQueueName;
    } UA_BrokerDataSetReaderTransportDataType;

PubSubConfigurationRefMask
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_PubSubConfigurationRefMask;
    
    #define UA_PUBSUBCONFIGURATIONREFMASK_NONE 0
    #define UA_PUBSUBCONFIGURATIONREFMASK_ELEMENTADD 1
    #define UA_PUBSUBCONFIGURATIONREFMASK_ELEMENTMATCH 2
    #define UA_PUBSUBCONFIGURATIONREFMASK_ELEMENTMODIFY 4
    #define UA_PUBSUBCONFIGURATIONREFMASK_ELEMENTREMOVE 8
    #define UA_PUBSUBCONFIGURATIONREFMASK_REFERENCEWRITER 16
    #define UA_PUBSUBCONFIGURATIONREFMASK_REFERENCEREADER 32
    #define UA_PUBSUBCONFIGURATIONREFMASK_REFERENCEWRITERGROUP 64
    #define UA_PUBSUBCONFIGURATIONREFMASK_REFERENCEREADERGROUP 128
    #define UA_PUBSUBCONFIGURATIONREFMASK_REFERENCECONNECTION 256
    #define UA_PUBSUBCONFIGURATIONREFMASK_REFERENCEPUBDATASET 512
    #define UA_PUBSUBCONFIGURATIONREFMASK_REFERENCESUBDATASET 1024
    #define UA_PUBSUBCONFIGURATIONREFMASK_REFERENCESECURITYGROUP 2048
    #define UA_PUBSUBCONFIGURATIONREFMASK_REFERENCEPUSHTARGET 4096

PubSubConfigurationRefDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_PubSubConfigurationRefMask configurationMask;
        UA_UInt16 elementIndex;
        UA_UInt16 connectionIndex;
        UA_UInt16 groupIndex;
    } UA_PubSubConfigurationRefDataType;

PubSubConfigurationValueDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_PubSubConfigurationRefDataType configurationElement;
        UA_String name;
        UA_Variant identifier;
    } UA_PubSubConfigurationValueDataType;

DiagnosticsLevel
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_DIAGNOSTICSLEVEL_BASIC = 0,
        UA_DIAGNOSTICSLEVEL_ADVANCED = 1,
        UA_DIAGNOSTICSLEVEL_INFO = 2,
        UA_DIAGNOSTICSLEVEL_LOG = 3,
        UA_DIAGNOSTICSLEVEL_DEBUG = 4
    } UA_DiagnosticsLevel;

PubSubDiagnosticsCounterClassification
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_PUBSUBDIAGNOSTICSCOUNTERCLASSIFICATION_INFORMATION = 0,
        UA_PUBSUBDIAGNOSTICSCOUNTERCLASSIFICATION_ERROR = 1
    } UA_PubSubDiagnosticsCounterClassification;

AliasNameDataType
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_QualifiedName aliasName;
        size_t referencedNodesSize;
        UA_ExpandedNodeId *referencedNodes;
    } UA_AliasNameDataType;

PasswordOptionsMask
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_PasswordOptionsMask;
    
    #define UA_PASSWORDOPTIONSMASK_NONE 0
    #define UA_PASSWORDOPTIONSMASK_SUPPORTINITIALPASSWORDCHANGE 1
    #define UA_PASSWORDOPTIONSMASK_SUPPORTDISABLEUSER 2
    #define UA_PASSWORDOPTIONSMASK_SUPPORTDISABLEDELETEFORUSER 4
    #define UA_PASSWORDOPTIONSMASK_SUPPORTNOCHANGEFORUSER 8
    #define UA_PASSWORDOPTIONSMASK_SUPPORTDESCRIPTIONFORUSER 16
    #define UA_PASSWORDOPTIONSMASK_REQUIRESUPPERCASECHARACTERS 32
    #define UA_PASSWORDOPTIONSMASK_REQUIRESLOWERCASECHARACTERS 64
    #define UA_PASSWORDOPTIONSMASK_REQUIRESDIGITCHARACTERS 128
    #define UA_PASSWORDOPTIONSMASK_REQUIRESSPECIALCHARACTERS 256

UserConfigurationMask
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_UserConfigurationMask;
    
    #define UA_USERCONFIGURATIONMASK_NONE 0
    #define UA_USERCONFIGURATIONMASK_NODELETE 1
    #define UA_USERCONFIGURATIONMASK_DISABLED 2
    #define UA_USERCONFIGURATIONMASK_NOCHANGEBYUSER 4
    #define UA_USERCONFIGURATIONMASK_MUSTCHANGEPASSWORD 8

UserManagementDataType
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String userName;
        UA_UserConfigurationMask userConfiguration;
        UA_String description;
    } UA_UserManagementDataType;

Duplex
^^^^^^

.. code-block:: c

    typedef enum {
        UA_DUPLEX_FULL = 0,
        UA_DUPLEX_HALF = 1,
        UA_DUPLEX_UNKNOWN = 2
    } UA_Duplex;

InterfaceAdminStatus
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_INTERFACEADMINSTATUS_UP = 0,
        UA_INTERFACEADMINSTATUS_DOWN = 1,
        UA_INTERFACEADMINSTATUS_TESTING = 2
    } UA_InterfaceAdminStatus;

InterfaceOperStatus
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_INTERFACEOPERSTATUS_UP = 0,
        UA_INTERFACEOPERSTATUS_DOWN = 1,
        UA_INTERFACEOPERSTATUS_TESTING = 2,
        UA_INTERFACEOPERSTATUS_UNKNOWN = 3,
        UA_INTERFACEOPERSTATUS_DORMANT = 4,
        UA_INTERFACEOPERSTATUS_NOTPRESENT = 5,
        UA_INTERFACEOPERSTATUS_LOWERLAYERDOWN = 6
    } UA_InterfaceOperStatus;

NegotiationStatus
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_NEGOTIATIONSTATUS_INPROGRESS = 0,
        UA_NEGOTIATIONSTATUS_COMPLETE = 1,
        UA_NEGOTIATIONSTATUS_FAILED = 2,
        UA_NEGOTIATIONSTATUS_UNKNOWN = 3,
        UA_NEGOTIATIONSTATUS_NONEGOTIATION = 4
    } UA_NegotiationStatus;

TsnFailureCode
^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_TSNFAILURECODE_NOFAILURE = 0,
        UA_TSNFAILURECODE_INSUFFICIENTBANDWIDTH = 1,
        UA_TSNFAILURECODE_INSUFFICIENTRESOURCES = 2,
        UA_TSNFAILURECODE_INSUFFICIENTTRAFFICCLASSBANDWIDTH = 3,
        UA_TSNFAILURECODE_STREAMIDINUSE = 4,
        UA_TSNFAILURECODE_STREAMDESTINATIONADDRESSINUSE = 5,
        UA_TSNFAILURECODE_STREAMPREEMPTEDBYHIGHERRANK = 6,
        UA_TSNFAILURECODE_LATENCYHASCHANGED = 7,
        UA_TSNFAILURECODE_EGRESSPORTNOTAVBCAPABLE = 8,
        UA_TSNFAILURECODE_USEDIFFERENTDESTINATIONADDRESS = 9,
        UA_TSNFAILURECODE_OUTOFMSRPRESOURCES = 10,
        UA_TSNFAILURECODE_OUTOFMMRPRESOURCES = 11,
        UA_TSNFAILURECODE_CANNOTSTOREDESTINATIONADDRESS = 12,
        UA_TSNFAILURECODE_PRIORITYISNOTANSRCCLASS = 13,
        UA_TSNFAILURECODE_MAXFRAMESIZETOOLARGE = 14,
        UA_TSNFAILURECODE_MAXFANINPORTSLIMITREACHED = 15,
        UA_TSNFAILURECODE_FIRSTVALUECHANGEDFORSTREAMID = 16,
        UA_TSNFAILURECODE_VLANBLOCKEDONEGRESS = 17,
        UA_TSNFAILURECODE_VLANTAGGINGDISABLEDONEGRESS = 18,
        UA_TSNFAILURECODE_SRCLASSPRIORITYMISMATCH = 19,
        UA_TSNFAILURECODE_FEATURENOTPROPAGATED = 20,
        UA_TSNFAILURECODE_MAXLATENCYEXCEEDED = 21,
        UA_TSNFAILURECODE_BRIDGEDOESNOTPROVIDENETWORKID = 22,
        UA_TSNFAILURECODE_STREAMTRANSFORMNOTSUPPORTED = 23,
        UA_TSNFAILURECODE_STREAMIDTYPENOTSUPPORTED = 24,
        UA_TSNFAILURECODE_FEATURENOTSUPPORTED = 25
    } UA_TsnFailureCode;

TsnStreamState
^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_TSNSTREAMSTATE_DISABLED = 0,
        UA_TSNSTREAMSTATE_CONFIGURING = 1,
        UA_TSNSTREAMSTATE_READY = 2,
        UA_TSNSTREAMSTATE_OPERATIONAL = 3,
        UA_TSNSTREAMSTATE_ERROR = 4
    } UA_TsnStreamState;

TsnTalkerStatus
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_TSNTALKERSTATUS_NONE = 0,
        UA_TSNTALKERSTATUS_READY = 1,
        UA_TSNTALKERSTATUS_FAILED = 2
    } UA_TsnTalkerStatus;

TsnListenerStatus
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_TSNLISTENERSTATUS_NONE = 0,
        UA_TSNLISTENERSTATUS_READY = 1,
        UA_TSNLISTENERSTATUS_PARTIALFAILED = 2,
        UA_TSNLISTENERSTATUS_FAILED = 3
    } UA_TsnListenerStatus;

PriorityMappingEntryType
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String mappingUri;
        UA_String priorityLabel;
        UA_Byte priorityValue_PCP;
        UA_UInt32 priorityValue_DSCP;
    } UA_PriorityMappingEntryType;

IdType
^^^^^^

.. code-block:: c

    typedef enum {
        UA_IDTYPE_NUMERIC = 0,
        UA_IDTYPE_STRING = 1,
        UA_IDTYPE_GUID = 2,
        UA_IDTYPE_OPAQUE = 3
    } UA_IdType;

NodeClass
^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_NODECLASS_UNSPECIFIED = 0,
        UA_NODECLASS_OBJECT = 1,
        UA_NODECLASS_VARIABLE = 2,
        UA_NODECLASS_METHOD = 4,
        UA_NODECLASS_OBJECTTYPE = 8,
        UA_NODECLASS_VARIABLETYPE = 16,
        UA_NODECLASS_REFERENCETYPE = 32,
        UA_NODECLASS_DATATYPE = 64,
        UA_NODECLASS_VIEW = 128
    } UA_NodeClass;

PermissionType
^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_PermissionType;
    
    #define UA_PERMISSIONTYPE_NONE 0
    #define UA_PERMISSIONTYPE_BROWSE 1
    #define UA_PERMISSIONTYPE_READROLEPERMISSIONS 2
    #define UA_PERMISSIONTYPE_WRITEATTRIBUTE 4
    #define UA_PERMISSIONTYPE_WRITEROLEPERMISSIONS 8
    #define UA_PERMISSIONTYPE_WRITEHISTORIZING 16
    #define UA_PERMISSIONTYPE_READ 32
    #define UA_PERMISSIONTYPE_WRITE 64
    #define UA_PERMISSIONTYPE_READHISTORY 128
    #define UA_PERMISSIONTYPE_INSERTHISTORY 256
    #define UA_PERMISSIONTYPE_MODIFYHISTORY 512
    #define UA_PERMISSIONTYPE_DELETEHISTORY 1024
    #define UA_PERMISSIONTYPE_RECEIVEEVENTS 2048
    #define UA_PERMISSIONTYPE_CALL 4096
    #define UA_PERMISSIONTYPE_ADDREFERENCE 8192
    #define UA_PERMISSIONTYPE_REMOVEREFERENCE 16384
    #define UA_PERMISSIONTYPE_DELETENODE 32768
    #define UA_PERMISSIONTYPE_ADDNODE 65536

AccessLevelType
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_Byte UA_AccessLevelType;
    
    #define UA_ACCESSLEVELTYPE_NONE 0
    #define UA_ACCESSLEVELTYPE_CURRENTREAD 1
    #define UA_ACCESSLEVELTYPE_CURRENTWRITE 2
    #define UA_ACCESSLEVELTYPE_HISTORYREAD 4
    #define UA_ACCESSLEVELTYPE_HISTORYWRITE 8
    #define UA_ACCESSLEVELTYPE_SEMANTICCHANGE 16
    #define UA_ACCESSLEVELTYPE_STATUSWRITE 32
    #define UA_ACCESSLEVELTYPE_TIMESTAMPWRITE 64

AccessLevelExType
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_AccessLevelExType;
    
    #define UA_ACCESSLEVELEXTYPE_NONE 0
    #define UA_ACCESSLEVELEXTYPE_CURRENTREAD 1
    #define UA_ACCESSLEVELEXTYPE_CURRENTWRITE 2
    #define UA_ACCESSLEVELEXTYPE_HISTORYREAD 4
    #define UA_ACCESSLEVELEXTYPE_HISTORYWRITE 8
    #define UA_ACCESSLEVELEXTYPE_SEMANTICCHANGE 16
    #define UA_ACCESSLEVELEXTYPE_STATUSWRITE 32
    #define UA_ACCESSLEVELEXTYPE_TIMESTAMPWRITE 64
    #define UA_ACCESSLEVELEXTYPE_NONATOMICREAD 256
    #define UA_ACCESSLEVELEXTYPE_NONATOMICWRITE 512
    #define UA_ACCESSLEVELEXTYPE_WRITEFULLARRAYONLY 1024
    #define UA_ACCESSLEVELEXTYPE_NOSUBDATATYPES 2048
    #define UA_ACCESSLEVELEXTYPE_NONVOLATILE 4096
    #define UA_ACCESSLEVELEXTYPE_CONSTANT 8192

EventNotifierType
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_Byte UA_EventNotifierType;
    
    #define UA_EVENTNOTIFIERTYPE_NONE 0
    #define UA_EVENTNOTIFIERTYPE_SUBSCRIBETOEVENTS 1
    #define UA_EVENTNOTIFIERTYPE_HISTORYREAD 4
    #define UA_EVENTNOTIFIERTYPE_HISTORYWRITE 8

AccessRestrictionType
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt16 UA_AccessRestrictionType;
    
    #define UA_ACCESSRESTRICTIONTYPE_NONE 0
    #define UA_ACCESSRESTRICTIONTYPE_SIGNINGREQUIRED 1
    #define UA_ACCESSRESTRICTIONTYPE_ENCRYPTIONREQUIRED 2
    #define UA_ACCESSRESTRICTIONTYPE_SESSIONREQUIRED 4
    #define UA_ACCESSRESTRICTIONTYPE_APPLYRESTRICTIONSTOBROWSE 8

RolePermissionType
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId roleId;
        UA_PermissionType permissions;
    } UA_RolePermissionType;

StructureType
^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_STRUCTURETYPE_STRUCTURE = 0,
        UA_STRUCTURETYPE_STRUCTUREWITHOPTIONALFIELDS = 1,
        UA_STRUCTURETYPE_UNION = 2,
        UA_STRUCTURETYPE_STRUCTUREWITHSUBTYPEDVALUES = 3,
        UA_STRUCTURETYPE_UNIONWITHSUBTYPEDVALUES = 4
    } UA_StructureType;

StructureField
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        UA_LocalizedText description;
        UA_NodeId dataType;
        UA_Int32 valueRank;
        size_t arrayDimensionsSize;
        UA_UInt32 *arrayDimensions;
        UA_UInt32 maxStringLength;
        UA_Boolean isOptional;
    } UA_StructureField;

StructureDefinition
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId defaultEncodingId;
        UA_NodeId baseDataType;
        UA_StructureType structureType;
        size_t fieldsSize;
        UA_StructureField *fields;
    } UA_StructureDefinition;

ReferenceNode
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId referenceTypeId;
        UA_Boolean isInverse;
        UA_ExpandedNodeId targetId;
    } UA_ReferenceNode;

Argument
^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        UA_NodeId dataType;
        UA_Int32 valueRank;
        size_t arrayDimensionsSize;
        UA_UInt32 *arrayDimensions;
        UA_LocalizedText description;
    } UA_Argument;

EnumValueType
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Int64 value;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
    } UA_EnumValueType;

EnumField
^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Int64 value;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_String name;
    } UA_EnumField;

OptionSet
^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ByteString value;
        UA_ByteString validBits;
    } UA_OptionSet;

NormalizedString
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_String UA_NormalizedString;

DecimalString
^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_String UA_DecimalString;

DurationString
^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_String UA_DurationString;

TimeString
^^^^^^^^^^

.. code-block:: c

    typedef UA_String UA_TimeString;

DateString
^^^^^^^^^^

.. code-block:: c

    typedef UA_String UA_DateString;

Duration
^^^^^^^^

.. code-block:: c

    typedef UA_Double UA_Duration;

UtcTime
^^^^^^^

.. code-block:: c

    typedef UA_DateTime UA_UtcTime;

LocaleId
^^^^^^^^

.. code-block:: c

    typedef UA_String UA_LocaleId;

TimeZoneDataType
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Int16 offset;
        UA_Boolean daylightSavingInOffset;
    } UA_TimeZoneDataType;

Index
^^^^^

.. code-block:: c

    typedef UA_ByteString UA_Index;

IntegerId
^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_IntegerId;

ApplicationType
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_APPLICATIONTYPE_SERVER = 0,
        UA_APPLICATIONTYPE_CLIENT = 1,
        UA_APPLICATIONTYPE_CLIENTANDSERVER = 2,
        UA_APPLICATIONTYPE_DISCOVERYSERVER = 3
    } UA_ApplicationType;

ApplicationDescription
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String applicationUri;
        UA_String productUri;
        UA_LocalizedText applicationName;
        UA_ApplicationType applicationType;
        UA_String gatewayServerUri;
        UA_String discoveryProfileUri;
        size_t discoveryUrlsSize;
        UA_String *discoveryUrls;
    } UA_ApplicationDescription;

RequestHeader
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId authenticationToken;
        UA_DateTime timestamp;
        UA_UInt32 requestHandle;
        UA_UInt32 returnDiagnostics;
        UA_String auditEntryId;
        UA_UInt32 timeoutHint;
        UA_ExtensionObject additionalHeader;
    } UA_RequestHeader;

ResponseHeader
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_DateTime timestamp;
        UA_UInt32 requestHandle;
        UA_StatusCode serviceResult;
        UA_DiagnosticInfo serviceDiagnostics;
        size_t stringTableSize;
        UA_String *stringTable;
        UA_ExtensionObject additionalHeader;
    } UA_ResponseHeader;

VersionTime
^^^^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_VersionTime;

ServiceFault
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
    } UA_ServiceFault;

SessionlessInvokeRequestType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 urisVersion;
        size_t namespaceUrisSize;
        UA_String *namespaceUris;
        size_t serverUrisSize;
        UA_String *serverUris;
        size_t localeIdsSize;
        UA_String *localeIds;
        UA_UInt32 serviceId;
    } UA_SessionlessInvokeRequestType;

SessionlessInvokeResponseType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t namespaceUrisSize;
        UA_String *namespaceUris;
        size_t serverUrisSize;
        UA_String *serverUris;
        UA_UInt32 serviceId;
    } UA_SessionlessInvokeResponseType;

FindServersRequest
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_String endpointUrl;
        size_t localeIdsSize;
        UA_String *localeIds;
        size_t serverUrisSize;
        UA_String *serverUris;
    } UA_FindServersRequest;

FindServersResponse
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t serversSize;
        UA_ApplicationDescription *servers;
    } UA_FindServersResponse;

ServerOnNetwork
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 recordId;
        UA_String serverName;
        UA_String discoveryUrl;
        size_t serverCapabilitiesSize;
        UA_String *serverCapabilities;
    } UA_ServerOnNetwork;

FindServersOnNetworkRequest
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 startingRecordId;
        UA_UInt32 maxRecordsToReturn;
        size_t serverCapabilityFilterSize;
        UA_String *serverCapabilityFilter;
    } UA_FindServersOnNetworkRequest;

FindServersOnNetworkResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        UA_DateTime lastCounterResetTime;
        size_t serversSize;
        UA_ServerOnNetwork *servers;
    } UA_FindServersOnNetworkResponse;

ApplicationInstanceCertificate
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_ApplicationInstanceCertificate;

MessageSecurityMode
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_MESSAGESECURITYMODE_INVALID = 0,
        UA_MESSAGESECURITYMODE_NONE = 1,
        UA_MESSAGESECURITYMODE_SIGN = 2,
        UA_MESSAGESECURITYMODE_SIGNANDENCRYPT = 3
    } UA_MessageSecurityMode;

UserTokenType
^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_USERTOKENTYPE_ANONYMOUS = 0,
        UA_USERTOKENTYPE_USERNAME = 1,
        UA_USERTOKENTYPE_CERTIFICATE = 2,
        UA_USERTOKENTYPE_ISSUEDTOKEN = 3
    } UA_UserTokenType;

UserTokenPolicy
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String policyId;
        UA_UserTokenType tokenType;
        UA_String issuedTokenType;
        UA_String issuerEndpointUrl;
        UA_String securityPolicyUri;
    } UA_UserTokenPolicy;

EndpointDescription
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String endpointUrl;
        UA_ApplicationDescription server;
        UA_ByteString serverCertificate;
        UA_MessageSecurityMode securityMode;
        UA_String securityPolicyUri;
        size_t userIdentityTokensSize;
        UA_UserTokenPolicy *userIdentityTokens;
        UA_String transportProfileUri;
        UA_Byte securityLevel;
    } UA_EndpointDescription;

GetEndpointsRequest
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_String endpointUrl;
        size_t localeIdsSize;
        UA_String *localeIds;
        size_t profileUrisSize;
        UA_String *profileUris;
    } UA_GetEndpointsRequest;

GetEndpointsResponse
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t endpointsSize;
        UA_EndpointDescription *endpoints;
    } UA_GetEndpointsResponse;

RegisteredServer
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String serverUri;
        UA_String productUri;
        size_t serverNamesSize;
        UA_LocalizedText *serverNames;
        UA_ApplicationType serverType;
        UA_String gatewayServerUri;
        size_t discoveryUrlsSize;
        UA_String *discoveryUrls;
        UA_String semaphoreFilePath;
        UA_Boolean isOnline;
    } UA_RegisteredServer;

RegisterServerRequest
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_RegisteredServer server;
    } UA_RegisterServerRequest;

RegisterServerResponse
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
    } UA_RegisterServerResponse;

MdnsDiscoveryConfiguration
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String mdnsServerName;
        size_t serverCapabilitiesSize;
        UA_String *serverCapabilities;
    } UA_MdnsDiscoveryConfiguration;

RegisterServer2Request
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_RegisteredServer server;
        size_t discoveryConfigurationSize;
        UA_ExtensionObject *discoveryConfiguration;
    } UA_RegisterServer2Request;

RegisterServer2Response
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t configurationResultsSize;
        UA_StatusCode *configurationResults;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_RegisterServer2Response;

SecurityTokenRequestType
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_SECURITYTOKENREQUESTTYPE_ISSUE = 0,
        UA_SECURITYTOKENREQUESTTYPE_RENEW = 1
    } UA_SecurityTokenRequestType;

ChannelSecurityToken
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 channelId;
        UA_UInt32 tokenId;
        UA_DateTime createdAt;
        UA_UInt32 revisedLifetime;
    } UA_ChannelSecurityToken;

OpenSecureChannelRequest
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 clientProtocolVersion;
        UA_SecurityTokenRequestType requestType;
        UA_MessageSecurityMode securityMode;
        UA_ByteString clientNonce;
        UA_UInt32 requestedLifetime;
    } UA_OpenSecureChannelRequest;

OpenSecureChannelResponse
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        UA_UInt32 serverProtocolVersion;
        UA_ChannelSecurityToken securityToken;
        UA_ByteString serverNonce;
    } UA_OpenSecureChannelResponse;

CloseSecureChannelRequest
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
    } UA_CloseSecureChannelRequest;

CloseSecureChannelResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
    } UA_CloseSecureChannelResponse;

SignedSoftwareCertificate
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ByteString certificateData;
        UA_ByteString signature;
    } UA_SignedSoftwareCertificate;

SessionAuthenticationToken
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_NodeId UA_SessionAuthenticationToken;

SignatureData
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String algorithm;
        UA_ByteString signature;
    } UA_SignatureData;

CreateSessionRequest
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_ApplicationDescription clientDescription;
        UA_String serverUri;
        UA_String endpointUrl;
        UA_String sessionName;
        UA_ByteString clientNonce;
        UA_ByteString clientCertificate;
        UA_Double requestedSessionTimeout;
        UA_UInt32 maxResponseMessageSize;
    } UA_CreateSessionRequest;

CreateSessionResponse
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        UA_NodeId sessionId;
        UA_NodeId authenticationToken;
        UA_Double revisedSessionTimeout;
        UA_ByteString serverNonce;
        UA_ByteString serverCertificate;
        size_t serverEndpointsSize;
        UA_EndpointDescription *serverEndpoints;
        size_t serverSoftwareCertificatesSize;
        UA_SignedSoftwareCertificate *serverSoftwareCertificates;
        UA_SignatureData serverSignature;
        UA_UInt32 maxRequestMessageSize;
    } UA_CreateSessionResponse;

UserIdentityToken
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String policyId;
    } UA_UserIdentityToken;

AnonymousIdentityToken
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String policyId;
    } UA_AnonymousIdentityToken;

UserNameIdentityToken
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String policyId;
        UA_String userName;
        UA_ByteString password;
        UA_String encryptionAlgorithm;
    } UA_UserNameIdentityToken;

X509IdentityToken
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String policyId;
        UA_ByteString certificateData;
    } UA_X509IdentityToken;

IssuedIdentityToken
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String policyId;
        UA_ByteString tokenData;
        UA_String encryptionAlgorithm;
    } UA_IssuedIdentityToken;

RsaEncryptedSecret
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_RsaEncryptedSecret;

EccEncryptedSecret
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_EccEncryptedSecret;

ActivateSessionRequest
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_SignatureData clientSignature;
        size_t clientSoftwareCertificatesSize;
        UA_SignedSoftwareCertificate *clientSoftwareCertificates;
        size_t localeIdsSize;
        UA_String *localeIds;
        UA_ExtensionObject userIdentityToken;
        UA_SignatureData userTokenSignature;
    } UA_ActivateSessionRequest;

ActivateSessionResponse
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        UA_ByteString serverNonce;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_ActivateSessionResponse;

CloseSessionRequest
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_Boolean deleteSubscriptions;
    } UA_CloseSessionRequest;

CloseSessionResponse
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
    } UA_CloseSessionResponse;

CancelRequest
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 requestHandle;
    } UA_CancelRequest;

CancelResponse
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        UA_UInt32 cancelCount;
    } UA_CancelResponse;

NodeAttributesMask
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_NODEATTRIBUTESMASK_NONE = 0,
        UA_NODEATTRIBUTESMASK_ACCESSLEVEL = 1,
        UA_NODEATTRIBUTESMASK_ARRAYDIMENSIONS = 2,
        UA_NODEATTRIBUTESMASK_BROWSENAME = 4,
        UA_NODEATTRIBUTESMASK_CONTAINSNOLOOPS = 8,
        UA_NODEATTRIBUTESMASK_DATATYPE = 16,
        UA_NODEATTRIBUTESMASK_DESCRIPTION = 32,
        UA_NODEATTRIBUTESMASK_DISPLAYNAME = 64,
        UA_NODEATTRIBUTESMASK_EVENTNOTIFIER = 128,
        UA_NODEATTRIBUTESMASK_EXECUTABLE = 256,
        UA_NODEATTRIBUTESMASK_HISTORIZING = 512,
        UA_NODEATTRIBUTESMASK_INVERSENAME = 1024,
        UA_NODEATTRIBUTESMASK_ISABSTRACT = 2048,
        UA_NODEATTRIBUTESMASK_MINIMUMSAMPLINGINTERVAL = 4096,
        UA_NODEATTRIBUTESMASK_NODECLASS = 8192,
        UA_NODEATTRIBUTESMASK_NODEID = 16384,
        UA_NODEATTRIBUTESMASK_SYMMETRIC = 32768,
        UA_NODEATTRIBUTESMASK_USERACCESSLEVEL = 65536,
        UA_NODEATTRIBUTESMASK_USEREXECUTABLE = 131072,
        UA_NODEATTRIBUTESMASK_USERWRITEMASK = 262144,
        UA_NODEATTRIBUTESMASK_VALUERANK = 524288,
        UA_NODEATTRIBUTESMASK_WRITEMASK = 1048576,
        UA_NODEATTRIBUTESMASK_VALUE = 2097152,
        UA_NODEATTRIBUTESMASK_DATATYPEDEFINITION = 4194304,
        UA_NODEATTRIBUTESMASK_ROLEPERMISSIONS = 8388608,
        UA_NODEATTRIBUTESMASK_ACCESSRESTRICTIONS = 16777216,
        UA_NODEATTRIBUTESMASK_ALL = 33554431,
        UA_NODEATTRIBUTESMASK_BASENODE = 26501220,
        UA_NODEATTRIBUTESMASK_OBJECT = 26501348,
        UA_NODEATTRIBUTESMASK_OBJECTTYPE = 26503268,
        UA_NODEATTRIBUTESMASK_VARIABLE = 26571383,
        UA_NODEATTRIBUTESMASK_VARIABLETYPE = 28600438,
        UA_NODEATTRIBUTESMASK_METHOD = 26632548,
        UA_NODEATTRIBUTESMASK_REFERENCETYPE = 26537060,
        UA_NODEATTRIBUTESMASK_VIEW = 26501356
    } UA_NodeAttributesMask;

NodeAttributes
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
    } UA_NodeAttributes;

ObjectAttributes
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
        UA_Byte eventNotifier;
    } UA_ObjectAttributes;

VariableAttributes
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
        UA_Variant value;
        UA_NodeId dataType;
        UA_Int32 valueRank;
        size_t arrayDimensionsSize;
        UA_UInt32 *arrayDimensions;
        UA_Byte accessLevel;
        UA_Byte userAccessLevel;
        UA_Double minimumSamplingInterval;
        UA_Boolean historizing;
    } UA_VariableAttributes;

MethodAttributes
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
        UA_Boolean executable;
        UA_Boolean userExecutable;
    } UA_MethodAttributes;

ObjectTypeAttributes
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
        UA_Boolean isAbstract;
    } UA_ObjectTypeAttributes;

VariableTypeAttributes
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
        UA_Variant value;
        UA_NodeId dataType;
        UA_Int32 valueRank;
        size_t arrayDimensionsSize;
        UA_UInt32 *arrayDimensions;
        UA_Boolean isAbstract;
    } UA_VariableTypeAttributes;

ReferenceTypeAttributes
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
        UA_Boolean isAbstract;
        UA_Boolean symmetric;
        UA_LocalizedText inverseName;
    } UA_ReferenceTypeAttributes;

DataTypeAttributes
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
        UA_Boolean isAbstract;
    } UA_DataTypeAttributes;

ViewAttributes
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
        UA_Boolean containsNoLoops;
        UA_Byte eventNotifier;
    } UA_ViewAttributes;

GenericAttributeValue
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 attributeId;
        UA_Variant value;
    } UA_GenericAttributeValue;

GenericAttributes
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 specifiedAttributes;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
        UA_UInt32 writeMask;
        UA_UInt32 userWriteMask;
        size_t attributeValuesSize;
        UA_GenericAttributeValue *attributeValues;
    } UA_GenericAttributes;

AddNodesItem
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ExpandedNodeId parentNodeId;
        UA_NodeId referenceTypeId;
        UA_ExpandedNodeId requestedNewNodeId;
        UA_QualifiedName browseName;
        UA_NodeClass nodeClass;
        UA_ExtensionObject nodeAttributes;
        UA_ExpandedNodeId typeDefinition;
    } UA_AddNodesItem;

AddNodesResult
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        UA_NodeId addedNodeId;
    } UA_AddNodesResult;

AddNodesRequest
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t nodesToAddSize;
        UA_AddNodesItem *nodesToAdd;
    } UA_AddNodesRequest;

AddNodesResponse
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_AddNodesResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_AddNodesResponse;

AddReferencesItem
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId sourceNodeId;
        UA_NodeId referenceTypeId;
        UA_Boolean isForward;
        UA_String targetServerUri;
        UA_ExpandedNodeId targetNodeId;
        UA_NodeClass targetNodeClass;
    } UA_AddReferencesItem;

AddReferencesRequest
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t referencesToAddSize;
        UA_AddReferencesItem *referencesToAdd;
    } UA_AddReferencesRequest;

AddReferencesResponse
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_AddReferencesResponse;

DeleteNodesItem
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_Boolean deleteTargetReferences;
    } UA_DeleteNodesItem;

DeleteNodesRequest
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t nodesToDeleteSize;
        UA_DeleteNodesItem *nodesToDelete;
    } UA_DeleteNodesRequest;

DeleteNodesResponse
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_DeleteNodesResponse;

DeleteReferencesItem
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId sourceNodeId;
        UA_NodeId referenceTypeId;
        UA_Boolean isForward;
        UA_ExpandedNodeId targetNodeId;
        UA_Boolean deleteBidirectional;
    } UA_DeleteReferencesItem;

DeleteReferencesRequest
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t referencesToDeleteSize;
        UA_DeleteReferencesItem *referencesToDelete;
    } UA_DeleteReferencesRequest;

DeleteReferencesResponse
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_DeleteReferencesResponse;

AttributeWriteMask
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_AttributeWriteMask;
    
    #define UA_ATTRIBUTEWRITEMASK_NONE 0
    #define UA_ATTRIBUTEWRITEMASK_ACCESSLEVEL 1
    #define UA_ATTRIBUTEWRITEMASK_ARRAYDIMENSIONS 2
    #define UA_ATTRIBUTEWRITEMASK_BROWSENAME 4
    #define UA_ATTRIBUTEWRITEMASK_CONTAINSNOLOOPS 8
    #define UA_ATTRIBUTEWRITEMASK_DATATYPE 16
    #define UA_ATTRIBUTEWRITEMASK_DESCRIPTION 32
    #define UA_ATTRIBUTEWRITEMASK_DISPLAYNAME 64
    #define UA_ATTRIBUTEWRITEMASK_EVENTNOTIFIER 128
    #define UA_ATTRIBUTEWRITEMASK_EXECUTABLE 256
    #define UA_ATTRIBUTEWRITEMASK_HISTORIZING 512
    #define UA_ATTRIBUTEWRITEMASK_INVERSENAME 1024
    #define UA_ATTRIBUTEWRITEMASK_ISABSTRACT 2048
    #define UA_ATTRIBUTEWRITEMASK_MINIMUMSAMPLINGINTERVAL 4096
    #define UA_ATTRIBUTEWRITEMASK_NODECLASS 8192
    #define UA_ATTRIBUTEWRITEMASK_NODEID 16384
    #define UA_ATTRIBUTEWRITEMASK_SYMMETRIC 32768
    #define UA_ATTRIBUTEWRITEMASK_USERACCESSLEVEL 65536
    #define UA_ATTRIBUTEWRITEMASK_USEREXECUTABLE 131072
    #define UA_ATTRIBUTEWRITEMASK_USERWRITEMASK 262144
    #define UA_ATTRIBUTEWRITEMASK_VALUERANK 524288
    #define UA_ATTRIBUTEWRITEMASK_WRITEMASK 1048576
    #define UA_ATTRIBUTEWRITEMASK_VALUEFORVARIABLETYPE 2097152
    #define UA_ATTRIBUTEWRITEMASK_DATATYPEDEFINITION 4194304
    #define UA_ATTRIBUTEWRITEMASK_ROLEPERMISSIONS 8388608
    #define UA_ATTRIBUTEWRITEMASK_ACCESSRESTRICTIONS 16777216
    #define UA_ATTRIBUTEWRITEMASK_ACCESSLEVELEX 33554432

BrowseDirection
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_BROWSEDIRECTION_FORWARD = 0,
        UA_BROWSEDIRECTION_INVERSE = 1,
        UA_BROWSEDIRECTION_BOTH = 2,
        UA_BROWSEDIRECTION_INVALID = 3
    } UA_BrowseDirection;

ViewDescription
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId viewId;
        UA_DateTime timestamp;
        UA_UInt32 viewVersion;
    } UA_ViewDescription;

BrowseDescription
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_BrowseDirection browseDirection;
        UA_NodeId referenceTypeId;
        UA_Boolean includeSubtypes;
        UA_UInt32 nodeClassMask;
        UA_UInt32 resultMask;
    } UA_BrowseDescription;

BrowseResultMask
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_BROWSERESULTMASK_NONE = 0,
        UA_BROWSERESULTMASK_REFERENCETYPEID = 1,
        UA_BROWSERESULTMASK_ISFORWARD = 2,
        UA_BROWSERESULTMASK_NODECLASS = 4,
        UA_BROWSERESULTMASK_BROWSENAME = 8,
        UA_BROWSERESULTMASK_DISPLAYNAME = 16,
        UA_BROWSERESULTMASK_TYPEDEFINITION = 32,
        UA_BROWSERESULTMASK_ALL = 63,
        UA_BROWSERESULTMASK_REFERENCETYPEINFO = 3,
        UA_BROWSERESULTMASK_TARGETINFO = 60
    } UA_BrowseResultMask;

ReferenceDescription
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId referenceTypeId;
        UA_Boolean isForward;
        UA_ExpandedNodeId nodeId;
        UA_QualifiedName browseName;
        UA_LocalizedText displayName;
        UA_NodeClass nodeClass;
        UA_ExpandedNodeId typeDefinition;
    } UA_ReferenceDescription;

ContinuationPoint
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_ByteString UA_ContinuationPoint;

BrowseResult
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        UA_ByteString continuationPoint;
        size_t referencesSize;
        UA_ReferenceDescription *references;
    } UA_BrowseResult;

BrowseRequest
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_ViewDescription view;
        UA_UInt32 requestedMaxReferencesPerNode;
        size_t nodesToBrowseSize;
        UA_BrowseDescription *nodesToBrowse;
    } UA_BrowseRequest;

BrowseResponse
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_BrowseResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_BrowseResponse;

BrowseNextRequest
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_Boolean releaseContinuationPoints;
        size_t continuationPointsSize;
        UA_ByteString *continuationPoints;
    } UA_BrowseNextRequest;

BrowseNextResponse
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_BrowseResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_BrowseNextResponse;

RelativePathElement
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId referenceTypeId;
        UA_Boolean isInverse;
        UA_Boolean includeSubtypes;
        UA_QualifiedName targetName;
    } UA_RelativePathElement;

RelativePath
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t elementsSize;
        UA_RelativePathElement *elements;
    } UA_RelativePath;

BrowsePath
^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId startingNode;
        UA_RelativePath relativePath;
    } UA_BrowsePath;

BrowsePathTarget
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ExpandedNodeId targetId;
        UA_UInt32 remainingPathIndex;
    } UA_BrowsePathTarget;

BrowsePathResult
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        size_t targetsSize;
        UA_BrowsePathTarget *targets;
    } UA_BrowsePathResult;

TranslateBrowsePathsToNodeIdsRequest
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t browsePathsSize;
        UA_BrowsePath *browsePaths;
    } UA_TranslateBrowsePathsToNodeIdsRequest;

TranslateBrowsePathsToNodeIdsResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_BrowsePathResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_TranslateBrowsePathsToNodeIdsResponse;

RegisterNodesRequest
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t nodesToRegisterSize;
        UA_NodeId *nodesToRegister;
    } UA_RegisterNodesRequest;

RegisterNodesResponse
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t registeredNodeIdsSize;
        UA_NodeId *registeredNodeIds;
    } UA_RegisterNodesResponse;

UnregisterNodesRequest
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t nodesToUnregisterSize;
        UA_NodeId *nodesToUnregister;
    } UA_UnregisterNodesRequest;

UnregisterNodesResponse
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
    } UA_UnregisterNodesResponse;

Counter
^^^^^^^

.. code-block:: c

    typedef UA_UInt32 UA_Counter;

OpaqueNumericRange
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef UA_String UA_OpaqueNumericRange;

EndpointConfiguration
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Int32 operationTimeout;
        UA_Boolean useBinaryEncoding;
        UA_Int32 maxStringLength;
        UA_Int32 maxByteStringLength;
        UA_Int32 maxArrayLength;
        UA_Int32 maxMessageSize;
        UA_Int32 maxBufferSize;
        UA_Int32 channelLifetime;
        UA_Int32 securityTokenLifetime;
    } UA_EndpointConfiguration;

QueryDataDescription
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RelativePath relativePath;
        UA_UInt32 attributeId;
        UA_String indexRange;
    } UA_QueryDataDescription;

NodeTypeDescription
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ExpandedNodeId typeDefinitionNode;
        UA_Boolean includeSubTypes;
        size_t dataToReturnSize;
        UA_QueryDataDescription *dataToReturn;
    } UA_NodeTypeDescription;

FilterOperator
^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_FILTEROPERATOR_EQUALS = 0,
        UA_FILTEROPERATOR_ISNULL = 1,
        UA_FILTEROPERATOR_GREATERTHAN = 2,
        UA_FILTEROPERATOR_LESSTHAN = 3,
        UA_FILTEROPERATOR_GREATERTHANOREQUAL = 4,
        UA_FILTEROPERATOR_LESSTHANOREQUAL = 5,
        UA_FILTEROPERATOR_LIKE = 6,
        UA_FILTEROPERATOR_NOT = 7,
        UA_FILTEROPERATOR_BETWEEN = 8,
        UA_FILTEROPERATOR_INLIST = 9,
        UA_FILTEROPERATOR_AND = 10,
        UA_FILTEROPERATOR_OR = 11,
        UA_FILTEROPERATOR_CAST = 12,
        UA_FILTEROPERATOR_INVIEW = 13,
        UA_FILTEROPERATOR_OFTYPE = 14,
        UA_FILTEROPERATOR_RELATEDTO = 15,
        UA_FILTEROPERATOR_BITWISEAND = 16,
        UA_FILTEROPERATOR_BITWISEOR = 17
    } UA_FilterOperator;

QueryDataSet
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ExpandedNodeId nodeId;
        UA_ExpandedNodeId typeDefinitionNode;
        size_t valuesSize;
        UA_Variant *values;
    } UA_QueryDataSet;

NodeReference
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_NodeId referenceTypeId;
        UA_Boolean isForward;
        size_t referencedNodeIdsSize;
        UA_NodeId *referencedNodeIds;
    } UA_NodeReference;

ContentFilterElement
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_FilterOperator filterOperator;
        size_t filterOperandsSize;
        UA_ExtensionObject *filterOperands;
    } UA_ContentFilterElement;

ContentFilter
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t elementsSize;
        UA_ContentFilterElement *elements;
    } UA_ContentFilter;

ElementOperand
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 index;
    } UA_ElementOperand;

LiteralOperand
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Variant value;
    } UA_LiteralOperand;

AttributeOperand
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_String alias;
        UA_RelativePath browsePath;
        UA_UInt32 attributeId;
        UA_String indexRange;
    } UA_AttributeOperand;

SimpleAttributeOperand
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId typeDefinitionId;
        size_t browsePathSize;
        UA_QualifiedName *browsePath;
        UA_UInt32 attributeId;
        UA_String indexRange;
    } UA_SimpleAttributeOperand;

ContentFilterElementResult
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        size_t operandStatusCodesSize;
        UA_StatusCode *operandStatusCodes;
        size_t operandDiagnosticInfosSize;
        UA_DiagnosticInfo *operandDiagnosticInfos;
    } UA_ContentFilterElementResult;

ContentFilterResult
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t elementResultsSize;
        UA_ContentFilterElementResult *elementResults;
        size_t elementDiagnosticInfosSize;
        UA_DiagnosticInfo *elementDiagnosticInfos;
    } UA_ContentFilterResult;

ParsingResult
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        size_t dataStatusCodesSize;
        UA_StatusCode *dataStatusCodes;
        size_t dataDiagnosticInfosSize;
        UA_DiagnosticInfo *dataDiagnosticInfos;
    } UA_ParsingResult;

QueryFirstRequest
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_ViewDescription view;
        size_t nodeTypesSize;
        UA_NodeTypeDescription *nodeTypes;
        UA_ContentFilter filter;
        UA_UInt32 maxDataSetsToReturn;
        UA_UInt32 maxReferencesToReturn;
    } UA_QueryFirstRequest;

QueryFirstResponse
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t queryDataSetsSize;
        UA_QueryDataSet *queryDataSets;
        UA_ByteString continuationPoint;
        size_t parsingResultsSize;
        UA_ParsingResult *parsingResults;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
        UA_ContentFilterResult filterResult;
    } UA_QueryFirstResponse;

QueryNextRequest
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_Boolean releaseContinuationPoint;
        UA_ByteString continuationPoint;
    } UA_QueryNextRequest;

QueryNextResponse
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t queryDataSetsSize;
        UA_QueryDataSet *queryDataSets;
        UA_ByteString revisedContinuationPoint;
    } UA_QueryNextResponse;

TimestampsToReturn
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_TIMESTAMPSTORETURN_SOURCE = 0,
        UA_TIMESTAMPSTORETURN_SERVER = 1,
        UA_TIMESTAMPSTORETURN_BOTH = 2,
        UA_TIMESTAMPSTORETURN_NEITHER = 3,
        UA_TIMESTAMPSTORETURN_INVALID = 4
    } UA_TimestampsToReturn;

ReadValueId
^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_UInt32 attributeId;
        UA_String indexRange;
        UA_QualifiedName dataEncoding;
    } UA_ReadValueId;

ReadRequest
^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_Double maxAge;
        UA_TimestampsToReturn timestampsToReturn;
        size_t nodesToReadSize;
        UA_ReadValueId *nodesToRead;
    } UA_ReadRequest;

ReadResponse
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_DataValue *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_ReadResponse;

HistoryReadValueId
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_String indexRange;
        UA_QualifiedName dataEncoding;
        UA_ByteString continuationPoint;
    } UA_HistoryReadValueId;

HistoryReadResult
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        UA_ByteString continuationPoint;
        UA_ExtensionObject historyData;
    } UA_HistoryReadResult;

ReadRawModifiedDetails
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Boolean isReadModified;
        UA_DateTime startTime;
        UA_DateTime endTime;
        UA_UInt32 numValuesPerNode;
        UA_Boolean returnBounds;
    } UA_ReadRawModifiedDetails;

ReadAtTimeDetails
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t reqTimesSize;
        UA_DateTime *reqTimes;
        UA_Boolean useSimpleBounds;
    } UA_ReadAtTimeDetails;

ReadAnnotationDataDetails
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t reqTimesSize;
        UA_DateTime *reqTimes;
    } UA_ReadAnnotationDataDetails;

HistoryData
^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t dataValuesSize;
        UA_DataValue *dataValues;
    } UA_HistoryData;

HistoryReadRequest
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_ExtensionObject historyReadDetails;
        UA_TimestampsToReturn timestampsToReturn;
        UA_Boolean releaseContinuationPoints;
        size_t nodesToReadSize;
        UA_HistoryReadValueId *nodesToRead;
    } UA_HistoryReadRequest;

HistoryReadResponse
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_HistoryReadResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_HistoryReadResponse;

WriteValue
^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_UInt32 attributeId;
        UA_String indexRange;
        UA_DataValue value;
    } UA_WriteValue;

WriteRequest
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t nodesToWriteSize;
        UA_WriteValue *nodesToWrite;
    } UA_WriteRequest;

WriteResponse
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_WriteResponse;

HistoryUpdateDetails
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
    } UA_HistoryUpdateDetails;

HistoryUpdateType
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_HISTORYUPDATETYPE_INSERT = 1,
        UA_HISTORYUPDATETYPE_REPLACE = 2,
        UA_HISTORYUPDATETYPE_UPDATE = 3,
        UA_HISTORYUPDATETYPE_DELETE = 4
    } UA_HistoryUpdateType;

PerformUpdateType
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_PERFORMUPDATETYPE_INSERT = 1,
        UA_PERFORMUPDATETYPE_REPLACE = 2,
        UA_PERFORMUPDATETYPE_UPDATE = 3,
        UA_PERFORMUPDATETYPE_REMOVE = 4
    } UA_PerformUpdateType;

UpdateDataDetails
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_PerformUpdateType performInsertReplace;
        size_t updateValuesSize;
        UA_DataValue *updateValues;
    } UA_UpdateDataDetails;

UpdateStructureDataDetails
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_PerformUpdateType performInsertReplace;
        size_t updateValuesSize;
        UA_DataValue *updateValues;
    } UA_UpdateStructureDataDetails;

DeleteRawModifiedDetails
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_Boolean isDeleteModified;
        UA_DateTime startTime;
        UA_DateTime endTime;
    } UA_DeleteRawModifiedDetails;

DeleteAtTimeDetails
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        size_t reqTimesSize;
        UA_DateTime *reqTimes;
    } UA_DeleteAtTimeDetails;

DeleteEventDetails
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        size_t eventIdsSize;
        UA_ByteString *eventIds;
    } UA_DeleteEventDetails;

HistoryUpdateResult
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        size_t operationResultsSize;
        UA_StatusCode *operationResults;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_HistoryUpdateResult;

HistoryUpdateRequest
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t historyUpdateDetailsSize;
        UA_ExtensionObject *historyUpdateDetails;
    } UA_HistoryUpdateRequest;

HistoryUpdateResponse
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_HistoryUpdateResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_HistoryUpdateResponse;

CallMethodRequest
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId objectId;
        UA_NodeId methodId;
        size_t inputArgumentsSize;
        UA_Variant *inputArguments;
    } UA_CallMethodRequest;

CallMethodResult
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        size_t inputArgumentResultsSize;
        UA_StatusCode *inputArgumentResults;
        size_t inputArgumentDiagnosticInfosSize;
        UA_DiagnosticInfo *inputArgumentDiagnosticInfos;
        size_t outputArgumentsSize;
        UA_Variant *outputArguments;
    } UA_CallMethodResult;

CallRequest
^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t methodsToCallSize;
        UA_CallMethodRequest *methodsToCall;
    } UA_CallRequest;

CallResponse
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_CallMethodResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_CallResponse;

MonitoringMode
^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_MONITORINGMODE_DISABLED = 0,
        UA_MONITORINGMODE_SAMPLING = 1,
        UA_MONITORINGMODE_REPORTING = 2
    } UA_MonitoringMode;

DataChangeTrigger
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_DATACHANGETRIGGER_STATUS = 0,
        UA_DATACHANGETRIGGER_STATUSVALUE = 1,
        UA_DATACHANGETRIGGER_STATUSVALUETIMESTAMP = 2
    } UA_DataChangeTrigger;

DeadbandType
^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_DEADBANDTYPE_NONE = 0,
        UA_DEADBANDTYPE_ABSOLUTE = 1,
        UA_DEADBANDTYPE_PERCENT = 2
    } UA_DeadbandType;

DataChangeFilter
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_DataChangeTrigger trigger;
        UA_UInt32 deadbandType;
        UA_Double deadbandValue;
    } UA_DataChangeFilter;

EventFilter
^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t selectClausesSize;
        UA_SimpleAttributeOperand *selectClauses;
        UA_ContentFilter whereClause;
    } UA_EventFilter;

AggregateConfiguration
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Boolean useServerCapabilitiesDefaults;
        UA_Boolean treatUncertainAsBad;
        UA_Byte percentDataBad;
        UA_Byte percentDataGood;
        UA_Boolean useSlopedExtrapolation;
    } UA_AggregateConfiguration;

AggregateFilter
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_DateTime startTime;
        UA_NodeId aggregateType;
        UA_Double processingInterval;
        UA_AggregateConfiguration aggregateConfiguration;
    } UA_AggregateFilter;

EventFilterResult
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t selectClauseResultsSize;
        UA_StatusCode *selectClauseResults;
        size_t selectClauseDiagnosticInfosSize;
        UA_DiagnosticInfo *selectClauseDiagnosticInfos;
        UA_ContentFilterResult whereClauseResult;
    } UA_EventFilterResult;

AggregateFilterResult
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_DateTime revisedStartTime;
        UA_Double revisedProcessingInterval;
        UA_AggregateConfiguration revisedAggregateConfiguration;
    } UA_AggregateFilterResult;

MonitoringParameters
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 clientHandle;
        UA_Double samplingInterval;
        UA_ExtensionObject filter;
        UA_UInt32 queueSize;
        UA_Boolean discardOldest;
    } UA_MonitoringParameters;

MonitoredItemCreateRequest
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ReadValueId itemToMonitor;
        UA_MonitoringMode monitoringMode;
        UA_MonitoringParameters requestedParameters;
    } UA_MonitoredItemCreateRequest;

MonitoredItemCreateResult
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        UA_UInt32 monitoredItemId;
        UA_Double revisedSamplingInterval;
        UA_UInt32 revisedQueueSize;
        UA_ExtensionObject filterResult;
    } UA_MonitoredItemCreateResult;

CreateMonitoredItemsRequest
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 subscriptionId;
        UA_TimestampsToReturn timestampsToReturn;
        size_t itemsToCreateSize;
        UA_MonitoredItemCreateRequest *itemsToCreate;
    } UA_CreateMonitoredItemsRequest;

CreateMonitoredItemsResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_MonitoredItemCreateResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_CreateMonitoredItemsResponse;

MonitoredItemModifyRequest
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 monitoredItemId;
        UA_MonitoringParameters requestedParameters;
    } UA_MonitoredItemModifyRequest;

MonitoredItemModifyResult
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        UA_Double revisedSamplingInterval;
        UA_UInt32 revisedQueueSize;
        UA_ExtensionObject filterResult;
    } UA_MonitoredItemModifyResult;

ModifyMonitoredItemsRequest
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 subscriptionId;
        UA_TimestampsToReturn timestampsToReturn;
        size_t itemsToModifySize;
        UA_MonitoredItemModifyRequest *itemsToModify;
    } UA_ModifyMonitoredItemsRequest;

ModifyMonitoredItemsResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_MonitoredItemModifyResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_ModifyMonitoredItemsResponse;

SetMonitoringModeRequest
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 subscriptionId;
        UA_MonitoringMode monitoringMode;
        size_t monitoredItemIdsSize;
        UA_UInt32 *monitoredItemIds;
    } UA_SetMonitoringModeRequest;

SetMonitoringModeResponse
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_SetMonitoringModeResponse;

SetTriggeringRequest
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 subscriptionId;
        UA_UInt32 triggeringItemId;
        size_t linksToAddSize;
        UA_UInt32 *linksToAdd;
        size_t linksToRemoveSize;
        UA_UInt32 *linksToRemove;
    } UA_SetTriggeringRequest;

SetTriggeringResponse
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t addResultsSize;
        UA_StatusCode *addResults;
        size_t addDiagnosticInfosSize;
        UA_DiagnosticInfo *addDiagnosticInfos;
        size_t removeResultsSize;
        UA_StatusCode *removeResults;
        size_t removeDiagnosticInfosSize;
        UA_DiagnosticInfo *removeDiagnosticInfos;
    } UA_SetTriggeringResponse;

DeleteMonitoredItemsRequest
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 subscriptionId;
        size_t monitoredItemIdsSize;
        UA_UInt32 *monitoredItemIds;
    } UA_DeleteMonitoredItemsRequest;

DeleteMonitoredItemsResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_DeleteMonitoredItemsResponse;

CreateSubscriptionRequest
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_Double requestedPublishingInterval;
        UA_UInt32 requestedLifetimeCount;
        UA_UInt32 requestedMaxKeepAliveCount;
        UA_UInt32 maxNotificationsPerPublish;
        UA_Boolean publishingEnabled;
        UA_Byte priority;
    } UA_CreateSubscriptionRequest;

CreateSubscriptionResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        UA_UInt32 subscriptionId;
        UA_Double revisedPublishingInterval;
        UA_UInt32 revisedLifetimeCount;
        UA_UInt32 revisedMaxKeepAliveCount;
    } UA_CreateSubscriptionResponse;

ModifySubscriptionRequest
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 subscriptionId;
        UA_Double requestedPublishingInterval;
        UA_UInt32 requestedLifetimeCount;
        UA_UInt32 requestedMaxKeepAliveCount;
        UA_UInt32 maxNotificationsPerPublish;
        UA_Byte priority;
    } UA_ModifySubscriptionRequest;

ModifySubscriptionResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        UA_Double revisedPublishingInterval;
        UA_UInt32 revisedLifetimeCount;
        UA_UInt32 revisedMaxKeepAliveCount;
    } UA_ModifySubscriptionResponse;

SetPublishingModeRequest
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_Boolean publishingEnabled;
        size_t subscriptionIdsSize;
        UA_UInt32 *subscriptionIds;
    } UA_SetPublishingModeRequest;

SetPublishingModeResponse
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_SetPublishingModeResponse;

NotificationMessage
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 sequenceNumber;
        UA_DateTime publishTime;
        size_t notificationDataSize;
        UA_ExtensionObject *notificationData;
    } UA_NotificationMessage;

MonitoredItemNotification
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 clientHandle;
        UA_DataValue value;
    } UA_MonitoredItemNotification;

EventFieldList
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 clientHandle;
        size_t eventFieldsSize;
        UA_Variant *eventFields;
    } UA_EventFieldList;

HistoryEventFieldList
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t eventFieldsSize;
        UA_Variant *eventFields;
    } UA_HistoryEventFieldList;

StatusChangeNotification
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode status;
        UA_DiagnosticInfo diagnosticInfo;
    } UA_StatusChangeNotification;

SubscriptionAcknowledgement
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 subscriptionId;
        UA_UInt32 sequenceNumber;
    } UA_SubscriptionAcknowledgement;

PublishRequest
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t subscriptionAcknowledgementsSize;
        UA_SubscriptionAcknowledgement *subscriptionAcknowledgements;
    } UA_PublishRequest;

PublishResponse
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        UA_UInt32 subscriptionId;
        size_t availableSequenceNumbersSize;
        UA_UInt32 *availableSequenceNumbers;
        UA_Boolean moreNotifications;
        UA_NotificationMessage notificationMessage;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_PublishResponse;

RepublishRequest
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        UA_UInt32 subscriptionId;
        UA_UInt32 retransmitSequenceNumber;
    } UA_RepublishRequest;

RepublishResponse
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        UA_NotificationMessage notificationMessage;
    } UA_RepublishResponse;

TransferResult
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        size_t availableSequenceNumbersSize;
        UA_UInt32 *availableSequenceNumbers;
    } UA_TransferResult;

TransferSubscriptionsRequest
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t subscriptionIdsSize;
        UA_UInt32 *subscriptionIds;
        UA_Boolean sendInitialValues;
    } UA_TransferSubscriptionsRequest;

TransferSubscriptionsResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_TransferResult *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_TransferSubscriptionsResponse;

DeleteSubscriptionsRequest
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_RequestHeader requestHeader;
        size_t subscriptionIdsSize;
        UA_UInt32 *subscriptionIds;
    } UA_DeleteSubscriptionsRequest;

DeleteSubscriptionsResponse
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_ResponseHeader responseHeader;
        size_t resultsSize;
        UA_StatusCode *results;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_DeleteSubscriptionsResponse;

BuildInfo
^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String productUri;
        UA_String manufacturerName;
        UA_String productName;
        UA_String softwareVersion;
        UA_String buildNumber;
        UA_DateTime buildDate;
    } UA_BuildInfo;

RedundancySupport
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_REDUNDANCYSUPPORT_NONE = 0,
        UA_REDUNDANCYSUPPORT_COLD = 1,
        UA_REDUNDANCYSUPPORT_WARM = 2,
        UA_REDUNDANCYSUPPORT_HOT = 3,
        UA_REDUNDANCYSUPPORT_TRANSPARENT = 4,
        UA_REDUNDANCYSUPPORT_HOTANDMIRRORED = 5
    } UA_RedundancySupport;

ServerState
^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_SERVERSTATE_RUNNING = 0,
        UA_SERVERSTATE_FAILED = 1,
        UA_SERVERSTATE_NOCONFIGURATION = 2,
        UA_SERVERSTATE_SUSPENDED = 3,
        UA_SERVERSTATE_SHUTDOWN = 4,
        UA_SERVERSTATE_TEST = 5,
        UA_SERVERSTATE_COMMUNICATIONFAULT = 6,
        UA_SERVERSTATE_UNKNOWN = 7
    } UA_ServerState;

RedundantServerDataType
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String serverId;
        UA_Byte serviceLevel;
        UA_ServerState serverState;
    } UA_RedundantServerDataType;

EndpointUrlListDataType
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t endpointUrlListSize;
        UA_String *endpointUrlList;
    } UA_EndpointUrlListDataType;

NetworkGroupDataType
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String serverUri;
        size_t networkPathsSize;
        UA_EndpointUrlListDataType *networkPaths;
    } UA_NetworkGroupDataType;

SamplingIntervalDiagnosticsDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Double samplingInterval;
        UA_UInt32 monitoredItemCount;
        UA_UInt32 maxMonitoredItemCount;
        UA_UInt32 disabledMonitoredItemCount;
    } UA_SamplingIntervalDiagnosticsDataType;

ServerDiagnosticsSummaryDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 serverViewCount;
        UA_UInt32 currentSessionCount;
        UA_UInt32 cumulatedSessionCount;
        UA_UInt32 securityRejectedSessionCount;
        UA_UInt32 rejectedSessionCount;
        UA_UInt32 sessionTimeoutCount;
        UA_UInt32 sessionAbortCount;
        UA_UInt32 currentSubscriptionCount;
        UA_UInt32 cumulatedSubscriptionCount;
        UA_UInt32 publishingIntervalCount;
        UA_UInt32 securityRejectedRequestsCount;
        UA_UInt32 rejectedRequestsCount;
    } UA_ServerDiagnosticsSummaryDataType;

ServerStatusDataType
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_DateTime startTime;
        UA_DateTime currentTime;
        UA_ServerState state;
        UA_BuildInfo buildInfo;
        UA_UInt32 secondsTillShutdown;
        UA_LocalizedText shutdownReason;
    } UA_ServerStatusDataType;

SessionSecurityDiagnosticsDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId sessionId;
        UA_String clientUserIdOfSession;
        size_t clientUserIdHistorySize;
        UA_String *clientUserIdHistory;
        UA_String authenticationMechanism;
        UA_String encoding;
        UA_String transportProtocol;
        UA_MessageSecurityMode securityMode;
        UA_String securityPolicyUri;
        UA_ByteString clientCertificate;
    } UA_SessionSecurityDiagnosticsDataType;

ServiceCounterDataType
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 totalCount;
        UA_UInt32 errorCount;
    } UA_ServiceCounterDataType;

StatusResult
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_StatusCode statusCode;
        UA_DiagnosticInfo diagnosticInfo;
    } UA_StatusResult;

SubscriptionDiagnosticsDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId sessionId;
        UA_UInt32 subscriptionId;
        UA_Byte priority;
        UA_Double publishingInterval;
        UA_UInt32 maxKeepAliveCount;
        UA_UInt32 maxLifetimeCount;
        UA_UInt32 maxNotificationsPerPublish;
        UA_Boolean publishingEnabled;
        UA_UInt32 modifyCount;
        UA_UInt32 enableCount;
        UA_UInt32 disableCount;
        UA_UInt32 republishRequestCount;
        UA_UInt32 republishMessageRequestCount;
        UA_UInt32 republishMessageCount;
        UA_UInt32 transferRequestCount;
        UA_UInt32 transferredToAltClientCount;
        UA_UInt32 transferredToSameClientCount;
        UA_UInt32 publishRequestCount;
        UA_UInt32 dataChangeNotificationsCount;
        UA_UInt32 eventNotificationsCount;
        UA_UInt32 notificationsCount;
        UA_UInt32 latePublishRequestCount;
        UA_UInt32 currentKeepAliveCount;
        UA_UInt32 currentLifetimeCount;
        UA_UInt32 unacknowledgedMessageCount;
        UA_UInt32 discardedMessageCount;
        UA_UInt32 monitoredItemCount;
        UA_UInt32 disabledMonitoredItemCount;
        UA_UInt32 monitoringQueueOverflowCount;
        UA_UInt32 nextSequenceNumber;
        UA_UInt32 eventQueueOverFlowCount;
    } UA_SubscriptionDiagnosticsDataType;

ModelChangeStructureVerbMask
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_MODELCHANGESTRUCTUREVERBMASK_NODEADDED = 1,
        UA_MODELCHANGESTRUCTUREVERBMASK_NODEDELETED = 2,
        UA_MODELCHANGESTRUCTUREVERBMASK_REFERENCEADDED = 4,
        UA_MODELCHANGESTRUCTUREVERBMASK_REFERENCEDELETED = 8,
        UA_MODELCHANGESTRUCTUREVERBMASK_DATATYPECHANGED = 16
    } UA_ModelChangeStructureVerbMask;

ModelChangeStructureDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId affected;
        UA_NodeId affectedType;
        UA_Byte verb;
    } UA_ModelChangeStructureDataType;

SemanticChangeStructureDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId affected;
        UA_NodeId affectedType;
    } UA_SemanticChangeStructureDataType;

Range
^^^^^

.. code-block:: c

    typedef struct {
        UA_Double low;
        UA_Double high;
    } UA_Range;

EUInformation
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String namespaceUri;
        UA_Int32 unitId;
        UA_LocalizedText displayName;
        UA_LocalizedText description;
    } UA_EUInformation;

AxisScaleEnumeration
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_AXISSCALEENUMERATION_LINEAR = 0,
        UA_AXISSCALEENUMERATION_LOG = 1,
        UA_AXISSCALEENUMERATION_LN = 2
    } UA_AxisScaleEnumeration;

ComplexNumberType
^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Float real;
        UA_Float imaginary;
    } UA_ComplexNumberType;

DoubleComplexNumberType
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Double real;
        UA_Double imaginary;
    } UA_DoubleComplexNumberType;

AxisInformation
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_EUInformation engineeringUnits;
        UA_Range eURange;
        UA_LocalizedText title;
        UA_AxisScaleEnumeration axisScaleType;
        size_t axisStepsSize;
        UA_Double *axisSteps;
    } UA_AxisInformation;

XVType
^^^^^^

.. code-block:: c

    typedef struct {
        UA_Double x;
        UA_Float value;
    } UA_XVType;

ProgramDiagnosticDataType
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId createSessionId;
        UA_String createClientName;
        UA_DateTime invocationCreationTime;
        UA_DateTime lastTransitionTime;
        UA_String lastMethodCall;
        UA_NodeId lastMethodSessionId;
        size_t lastMethodInputArgumentsSize;
        UA_Argument *lastMethodInputArguments;
        size_t lastMethodOutputArgumentsSize;
        UA_Argument *lastMethodOutputArguments;
        UA_DateTime lastMethodCallTime;
        UA_StatusResult lastMethodReturnStatus;
    } UA_ProgramDiagnosticDataType;

ProgramDiagnostic2DataType
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId createSessionId;
        UA_String createClientName;
        UA_DateTime invocationCreationTime;
        UA_DateTime lastTransitionTime;
        UA_String lastMethodCall;
        UA_NodeId lastMethodSessionId;
        size_t lastMethodInputArgumentsSize;
        UA_Argument *lastMethodInputArguments;
        size_t lastMethodOutputArgumentsSize;
        UA_Argument *lastMethodOutputArguments;
        size_t lastMethodInputValuesSize;
        UA_Variant *lastMethodInputValues;
        size_t lastMethodOutputValuesSize;
        UA_Variant *lastMethodOutputValues;
        UA_DateTime lastMethodCallTime;
        UA_StatusCode lastMethodReturnStatus;
    } UA_ProgramDiagnostic2DataType;

Annotation
^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String message;
        UA_String userName;
        UA_DateTime annotationTime;
    } UA_Annotation;

ExceptionDeviationFormat
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef enum {
        UA_EXCEPTIONDEVIATIONFORMAT_ABSOLUTEVALUE = 0,
        UA_EXCEPTIONDEVIATIONFORMAT_PERCENTOFVALUE = 1,
        UA_EXCEPTIONDEVIATIONFORMAT_PERCENTOFRANGE = 2,
        UA_EXCEPTIONDEVIATIONFORMAT_PERCENTOFEURANGE = 3,
        UA_EXCEPTIONDEVIATIONFORMAT_UNKNOWN = 4
    } UA_ExceptionDeviationFormat;

EndpointType
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String endpointUrl;
        UA_MessageSecurityMode securityMode;
        UA_String securityPolicyUri;
        UA_String transportProfileUri;
    } UA_EndpointType;

StructureDescription
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId dataTypeId;
        UA_QualifiedName name;
        UA_StructureDefinition structureDefinition;
    } UA_StructureDescription;

FieldMetaData
^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        UA_LocalizedText description;
        UA_DataSetFieldFlags fieldFlags;
        UA_Byte builtInType;
        UA_NodeId dataType;
        UA_Int32 valueRank;
        size_t arrayDimensionsSize;
        UA_UInt32 *arrayDimensions;
        UA_UInt32 maxStringLength;
        UA_Guid dataSetFieldId;
        size_t propertiesSize;
        UA_KeyValuePair *properties;
    } UA_FieldMetaData;

PublishedEventsDataType
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId eventNotifier;
        size_t selectedFieldsSize;
        UA_SimpleAttributeOperand *selectedFields;
        UA_ContentFilter filter;
    } UA_PublishedEventsDataType;

PubSubGroupDataType
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        UA_Boolean enabled;
        UA_MessageSecurityMode securityMode;
        UA_String securityGroupId;
        size_t securityKeyServicesSize;
        UA_EndpointDescription *securityKeyServices;
        UA_UInt32 maxNetworkMessageSize;
        size_t groupPropertiesSize;
        UA_KeyValuePair *groupProperties;
    } UA_PubSubGroupDataType;

WriterGroupDataType
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        UA_Boolean enabled;
        UA_MessageSecurityMode securityMode;
        UA_String securityGroupId;
        size_t securityKeyServicesSize;
        UA_EndpointDescription *securityKeyServices;
        UA_UInt32 maxNetworkMessageSize;
        size_t groupPropertiesSize;
        UA_KeyValuePair *groupProperties;
        UA_UInt16 writerGroupId;
        UA_Double publishingInterval;
        UA_Double keepAliveTime;
        UA_Byte priority;
        size_t localeIdsSize;
        UA_String *localeIds;
        UA_String headerLayoutUri;
        UA_ExtensionObject transportSettings;
        UA_ExtensionObject messageSettings;
        size_t dataSetWritersSize;
        UA_DataSetWriterDataType *dataSetWriters;
    } UA_WriterGroupDataType;

FieldTargetDataType
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_Guid dataSetFieldId;
        UA_String receiverIndexRange;
        UA_NodeId targetNodeId;
        UA_UInt32 attributeId;
        UA_String writeIndexRange;
        UA_OverrideValueHandling overrideValueHandling;
        UA_Variant overrideValue;
    } UA_FieldTargetDataType;

SubscribedDataSetMirrorDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String parentNodeName;
        size_t rolePermissionsSize;
        UA_RolePermissionType *rolePermissions;
    } UA_SubscribedDataSetMirrorDataType;

SecurityGroupDataType
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        size_t securityGroupFolderSize;
        UA_String *securityGroupFolder;
        UA_Double keyLifetime;
        UA_String securityPolicyUri;
        UA_UInt32 maxFutureKeyCount;
        UA_UInt32 maxPastKeyCount;
        UA_String securityGroupId;
        size_t rolePermissionsSize;
        UA_RolePermissionType *rolePermissions;
        size_t groupPropertiesSize;
        UA_KeyValuePair *groupProperties;
    } UA_SecurityGroupDataType;

PubSubKeyPushTargetDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String applicationUri;
        size_t pushTargetFolderSize;
        UA_String *pushTargetFolder;
        UA_String endpointUrl;
        UA_String securityPolicyUri;
        UA_UserTokenPolicy userTokenType;
        UA_UInt16 requestedKeyCount;
        UA_Double retryInterval;
        size_t pushTargetPropertiesSize;
        UA_KeyValuePair *pushTargetProperties;
        size_t securityGroupsSize;
        UA_String *securityGroups;
    } UA_PubSubKeyPushTargetDataType;

EnumDefinition
^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t fieldsSize;
        UA_EnumField *fields;
    } UA_EnumDefinition;

ReadEventDetails
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_UInt32 numValuesPerNode;
        UA_DateTime startTime;
        UA_DateTime endTime;
        UA_EventFilter filter;
    } UA_ReadEventDetails;

ReadProcessedDetails
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_DateTime startTime;
        UA_DateTime endTime;
        UA_Double processingInterval;
        size_t aggregateTypeSize;
        UA_NodeId *aggregateType;
        UA_AggregateConfiguration aggregateConfiguration;
    } UA_ReadProcessedDetails;

ModificationInfo
^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_DateTime modificationTime;
        UA_HistoryUpdateType updateType;
        UA_String userName;
    } UA_ModificationInfo;

HistoryModifiedData
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t dataValuesSize;
        UA_DataValue *dataValues;
        size_t modificationInfosSize;
        UA_ModificationInfo *modificationInfos;
    } UA_HistoryModifiedData;

HistoryEvent
^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t eventsSize;
        UA_HistoryEventFieldList *events;
    } UA_HistoryEvent;

UpdateEventDetails
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId nodeId;
        UA_PerformUpdateType performInsertReplace;
        UA_EventFilter filter;
        size_t eventDataSize;
        UA_HistoryEventFieldList *eventData;
    } UA_UpdateEventDetails;

DataChangeNotification
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t monitoredItemsSize;
        UA_MonitoredItemNotification *monitoredItems;
        size_t diagnosticInfosSize;
        UA_DiagnosticInfo *diagnosticInfos;
    } UA_DataChangeNotification;

EventNotificationList
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t eventsSize;
        UA_EventFieldList *events;
    } UA_EventNotificationList;

SessionDiagnosticsDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId sessionId;
        UA_String sessionName;
        UA_ApplicationDescription clientDescription;
        UA_String serverUri;
        UA_String endpointUrl;
        size_t localeIdsSize;
        UA_String *localeIds;
        UA_Double actualSessionTimeout;
        UA_UInt32 maxResponseMessageSize;
        UA_DateTime clientConnectionTime;
        UA_DateTime clientLastContactTime;
        UA_UInt32 currentSubscriptionsCount;
        UA_UInt32 currentMonitoredItemsCount;
        UA_UInt32 currentPublishRequestsInQueue;
        UA_ServiceCounterDataType totalRequestCount;
        UA_UInt32 unauthorizedRequestCount;
        UA_ServiceCounterDataType readCount;
        UA_ServiceCounterDataType historyReadCount;
        UA_ServiceCounterDataType writeCount;
        UA_ServiceCounterDataType historyUpdateCount;
        UA_ServiceCounterDataType callCount;
        UA_ServiceCounterDataType createMonitoredItemsCount;
        UA_ServiceCounterDataType modifyMonitoredItemsCount;
        UA_ServiceCounterDataType setMonitoringModeCount;
        UA_ServiceCounterDataType setTriggeringCount;
        UA_ServiceCounterDataType deleteMonitoredItemsCount;
        UA_ServiceCounterDataType createSubscriptionCount;
        UA_ServiceCounterDataType modifySubscriptionCount;
        UA_ServiceCounterDataType setPublishingModeCount;
        UA_ServiceCounterDataType publishCount;
        UA_ServiceCounterDataType republishCount;
        UA_ServiceCounterDataType transferSubscriptionsCount;
        UA_ServiceCounterDataType deleteSubscriptionsCount;
        UA_ServiceCounterDataType addNodesCount;
        UA_ServiceCounterDataType addReferencesCount;
        UA_ServiceCounterDataType deleteNodesCount;
        UA_ServiceCounterDataType deleteReferencesCount;
        UA_ServiceCounterDataType browseCount;
        UA_ServiceCounterDataType browseNextCount;
        UA_ServiceCounterDataType translateBrowsePathsToNodeIdsCount;
        UA_ServiceCounterDataType queryFirstCount;
        UA_ServiceCounterDataType queryNextCount;
        UA_ServiceCounterDataType registerNodesCount;
        UA_ServiceCounterDataType unregisterNodesCount;
    } UA_SessionDiagnosticsDataType;

EnumDescription
^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_NodeId dataTypeId;
        UA_QualifiedName name;
        UA_EnumDefinition enumDefinition;
        UA_Byte builtInType;
    } UA_EnumDescription;

UABinaryFileDataType
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t namespacesSize;
        UA_String *namespaces;
        size_t structureDataTypesSize;
        UA_StructureDescription *structureDataTypes;
        size_t enumDataTypesSize;
        UA_EnumDescription *enumDataTypes;
        size_t simpleDataTypesSize;
        UA_SimpleTypeDescription *simpleDataTypes;
        UA_String schemaLocation;
        size_t fileHeaderSize;
        UA_KeyValuePair *fileHeader;
        UA_Variant body;
    } UA_UABinaryFileDataType;

DataSetMetaDataType
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t namespacesSize;
        UA_String *namespaces;
        size_t structureDataTypesSize;
        UA_StructureDescription *structureDataTypes;
        size_t enumDataTypesSize;
        UA_EnumDescription *enumDataTypes;
        size_t simpleDataTypesSize;
        UA_SimpleTypeDescription *simpleDataTypes;
        UA_String name;
        UA_LocalizedText description;
        size_t fieldsSize;
        UA_FieldMetaData *fields;
        UA_Guid dataSetClassId;
        UA_ConfigurationVersionDataType configurationVersion;
    } UA_DataSetMetaDataType;

PublishedDataSetDataType
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        size_t dataSetFolderSize;
        UA_String *dataSetFolder;
        UA_DataSetMetaDataType dataSetMetaData;
        size_t extensionFieldsSize;
        UA_KeyValuePair *extensionFields;
        UA_ExtensionObject dataSetSource;
    } UA_PublishedDataSetDataType;

DataSetReaderDataType
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        UA_Boolean enabled;
        UA_Variant publisherId;
        UA_UInt16 writerGroupId;
        UA_UInt16 dataSetWriterId;
        UA_DataSetMetaDataType dataSetMetaData;
        UA_DataSetFieldContentMask dataSetFieldContentMask;
        UA_Double messageReceiveTimeout;
        UA_UInt32 keyFrameCount;
        UA_String headerLayoutUri;
        UA_MessageSecurityMode securityMode;
        UA_String securityGroupId;
        size_t securityKeyServicesSize;
        UA_EndpointDescription *securityKeyServices;
        size_t dataSetReaderPropertiesSize;
        UA_KeyValuePair *dataSetReaderProperties;
        UA_ExtensionObject transportSettings;
        UA_ExtensionObject messageSettings;
        UA_ExtensionObject subscribedDataSet;
    } UA_DataSetReaderDataType;

TargetVariablesDataType
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t targetVariablesSize;
        UA_FieldTargetDataType *targetVariables;
    } UA_TargetVariablesDataType;

StandaloneSubscribedDataSetDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        size_t dataSetFolderSize;
        UA_String *dataSetFolder;
        UA_DataSetMetaDataType dataSetMetaData;
        UA_ExtensionObject subscribedDataSet;
    } UA_StandaloneSubscribedDataSetDataType;

DataTypeSchemaHeader
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t namespacesSize;
        UA_String *namespaces;
        size_t structureDataTypesSize;
        UA_StructureDescription *structureDataTypes;
        size_t enumDataTypesSize;
        UA_EnumDescription *enumDataTypes;
        size_t simpleDataTypesSize;
        UA_SimpleTypeDescription *simpleDataTypes;
    } UA_DataTypeSchemaHeader;

ReaderGroupDataType
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        UA_Boolean enabled;
        UA_MessageSecurityMode securityMode;
        UA_String securityGroupId;
        size_t securityKeyServicesSize;
        UA_EndpointDescription *securityKeyServices;
        UA_UInt32 maxNetworkMessageSize;
        size_t groupPropertiesSize;
        UA_KeyValuePair *groupProperties;
        UA_ExtensionObject transportSettings;
        UA_ExtensionObject messageSettings;
        size_t dataSetReadersSize;
        UA_DataSetReaderDataType *dataSetReaders;
    } UA_ReaderGroupDataType;

PubSubConnectionDataType
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        UA_String name;
        UA_Boolean enabled;
        UA_Variant publisherId;
        UA_String transportProfileUri;
        UA_ExtensionObject address;
        size_t connectionPropertiesSize;
        UA_KeyValuePair *connectionProperties;
        UA_ExtensionObject transportSettings;
        size_t writerGroupsSize;
        UA_WriterGroupDataType *writerGroups;
        size_t readerGroupsSize;
        UA_ReaderGroupDataType *readerGroups;
    } UA_PubSubConnectionDataType;

PubSubConfigurationDataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t publishedDataSetsSize;
        UA_PublishedDataSetDataType *publishedDataSets;
        size_t connectionsSize;
        UA_PubSubConnectionDataType *connections;
        UA_Boolean enabled;
    } UA_PubSubConfigurationDataType;

PubSubConfiguration2DataType
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef struct {
        size_t publishedDataSetsSize;
        UA_PublishedDataSetDataType *publishedDataSets;
        size_t connectionsSize;
        UA_PubSubConnectionDataType *connections;
        UA_Boolean enabled;
        size_t subscribedDataSetsSize;
        UA_StandaloneSubscribedDataSetDataType *subscribedDataSets;
        size_t dataSetClassesSize;
        UA_DataSetMetaDataType *dataSetClasses;
        size_t defaultSecurityKeyServicesSize;
        UA_EndpointDescription *defaultSecurityKeyServices;
        size_t securityGroupsSize;
        UA_SecurityGroupDataType *securityGroups;
        size_t pubSubKeyPushTargetsSize;
        UA_PubSubKeyPushTargetDataType *pubSubKeyPushTargets;
        UA_UInt32 configurationVersion;
        size_t configurationPropertiesSize;
        UA_KeyValuePair *configurationProperties;
    } UA_PubSubConfiguration2DataType;

