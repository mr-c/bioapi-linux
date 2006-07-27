/*-----------------------------------------------------------------------
 * File: DAL_TYPEARRAY.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*
 * File contains the declaration and the definition of the templated class
 * DAL_TYPE_ARRAY
 */

#ifndef DAL_ARRAY_H
#define DAL_ARRAY_H

#include "dal_common.h"

#ifdef _WIN32
#pragma warning(disable:4710) /* remove inline expansion warning */
#endif

/*
 * Class Name : DAL_TYPE_ARRAY
 *
 * Description :  This is an array class that is used for arrays of ELEMENT_TYPE that
 * has a known number of elements at the beginning and uses CSSM_DB_RECORDTYPE as
 * the key. Internally, the class is currently implemented with a very simple linear
 * probing closed hash function. The assumption is that the number of recordtypes
 * should be small enough that it doesn't really matter what algorithm is used. A
 * VERY simple hash table was used just in case that assumption is slightly wronge.
 */
template <class ELEMENT_TYPE>
class DAL_TYPE_ARRAY
{
private:
	/*
	 * Defintion of a node in the array
	 */
	struct DAL_TYPE_ARRAY_NODE
	{
	private:
		ELEMENT_TYPE m_Data;
		CSSM_DB_RECORDTYPE m_RecordType;

	public:
		DAL_TYPE_ARRAY_NODE()
		{
			m_RecordType = DAL_RECORDTYPE_OOB;
		}

		~DAL_TYPE_ARRAY_NODE()
		{
		}

		ELEMENT_TYPE & neSetData(const CSSM_DB_RECORDTYPE RecordType)
		{
			ASSERT(m_RecordType == DAL_RECORDTYPE_OOB || m_RecordType == RecordType);
			m_RecordType = RecordType;
			return m_Data;
		}

		ELEMENT_TYPE& neGetData()
		{
			return m_Data;
		}

		CSSM_DB_RECORDTYPE neGetRecordType()
		{
			return m_RecordType;
		}
	};

	DAL_TYPE_ARRAY_NODE * m_rgData;
	uint32 m_NumRecordTypes;

public:
	/*
	 * Contructor, initializes to default data
	 */
	DAL_TYPE_ARRAY()
	{
		m_rgData = NULL;
		m_NumRecordTypes = 0;
	}

#ifdef _DEBUG
	/*
	 * Copy contructor, should not be used
	 */
	DAL_TYPE_ARRAY(const DAL_TYPE_ARRAY<ELEMENT_TYPE> &Source)
	{
		Source;
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
	}
#endif

	/*
	 * Deconstructor
	 */
	~DAL_TYPE_ARRAY()
	{
		if (m_rgData)
		{
			delete[]m_rgData;
			m_rgData = NULL;
		}
	}

	/* Function Name: DAL_TYPE_ARRAY<ELEMENT_TYPE>::Copy
	 * Description: Preforms an assignment to the current array from another
	 * array same as (*this) = Source;
	 */
	CSSM_RETURN Copy(const DAL_TYPE_ARRAY<ELEMENT_TYPE> &Source)
	{
		if (m_rgData)
		{
			delete []m_rgData;
			m_rgData = NULL;
		}
		m_NumRecordTypes = Source.m_NumRecordTypes;

		if (Source.m_NumRecordTypes)
		{
			uint32 i;

			m_rgData = new DAL_TYPE_ARRAY_NODE[Source.m_NumRecordTypes];
			if (!m_rgData)
			{
				return CSSMERR_DL_MEMORY_ERROR;
			}

			m_NumRecordTypes = Source.m_NumRecordTypes;

			for (i = 0; i < m_NumRecordTypes; i++)
				m_rgData[i] = Source.m_rgData[i];
		}

		return CSSM_OK;
	}

	/* Function Name: DAL_TYPE_ARRAY<ELEMENT_TYPE>::neGetByRecordType
	 * Description: Retrieves data from the array using RecordType as the index
	 * Paramaters:
	 *	 RecordType(input) : Index of the array
	 */
	ELEMENT_TYPE * neGetByRecordType(CSSM_DB_RECORDTYPE RecordType) const
	{
		ASSERT(m_NumRecordTypes);

		if (DAL_RECORDTYPE_OOB != RecordType)
		{
			const uint32 InitialPosition = RecordType % m_NumRecordTypes;
			uint32 Position = InitialPosition;

			do
			{
				ASSERT(Position < m_NumRecordTypes);

				if (m_rgData[Position].neGetRecordType() == RecordType)
					return &m_rgData[Position].neGetData();

				if (m_rgData[Position].neGetRecordType() == DAL_RECORDTYPE_OOB)
				{
					/* The element is not in the list */
					break;
				}

				Position = (Position + 1) % m_NumRecordTypes;
			}
			while (Position != InitialPosition);
		}

		return NULL;
	}

	/* Function Name: DAL_TYPE_ARRAY<ELEMENT_TYPE>::neGetByPosition
	 * Description: Retrieves data from the array using position as the index
	 * Paramaters:
	 *	 Position(input) : Element in the array to retieve (0 is the first element)
	 */
	ELEMENT_TYPE * neGetByPosition(uint32 Position) const
	{
		ASSERT(m_NumRecordTypes && Position < m_NumRecordTypes);

		if (Position >= m_NumRecordTypes || m_rgData[Position].neGetRecordType() == DAL_RECORDTYPE_OOB)
			return NULL;
		else
			return &m_rgData[Position].neGetData();
	}

	/* Function Name: DAL_TYPE_ARRAY<ELEMENT_TYPE>::neGetRecordType
	 * Description: Retrieves the recordtype at a position in the array
	 * Paramaters:
	 *	 Position(input) : Element in the array to retieve (0 is the first element)
	 */
	CSSM_DB_RECORDTYPE neGetRecordType(uint32 Position) const
	{
		ASSERT(m_NumRecordTypes);

		if (Position < m_NumRecordTypes)
			return m_rgData[Position].neGetRecordType();
		else
			return DAL_RECORDTYPE_OOB;
	}


	/* Function Name: DAL_TYPE_ARRAY<ELEMENT_TYPE>::neSetData
	 * Description: Preform assignment to a member of the array (Array[RecordType] = Element)
	 * Paramaters:
	 *	 RecordType(input) : Index of the array (i.e.: Array[RecordType])
	 *	 Element (input) : Data to insert into the array
	 */
	ELEMENT_TYPE& neSetData(const CSSM_DB_RECORDTYPE RecordType, uint32 * pPosition = NULL)
	{
		ASSERT(m_NumRecordTypes);

		const uint32 InitialPosition = RecordType % m_NumRecordTypes;
		uint32 Position = InitialPosition;

		do
		{
			ASSERT(Position < m_NumRecordTypes);

			if (m_rgData[Position].neGetRecordType() == DAL_RECORDTYPE_OOB ||
				m_rgData[Position].neGetRecordType() == RecordType)
			{
				if (pPosition != NULL) *pPosition = Position;
				return m_rgData[Position].neSetData(RecordType);
			}

			Position = (Position + 1) % m_NumRecordTypes;
		}
		while (Position != InitialPosition);

		/* SetData/Initialize was called too many times */
		DEBUG_BREAK(); /* Algorithm error, freeze execution (in debug) */
		return *((ELEMENT_TYPE *)NULL);
	}

	/* Function Name: DAL_TYPE_ARRAY<ELEMENT_TYPE>::Initialize
	 * Description: Initialize an array object by describing the maximum number of elements and
	 * allocating the required space
	 * Paramaters:
	 *	 NumRecordTypes (input) : Maximum number of records that the array can have
	 */
	CSSM_RETURN Initialize(uint32 NumRecordTypes)
	{
		ASSERT(m_NumRecordTypes == 0);

		m_rgData = new DAL_TYPE_ARRAY_NODE[NumRecordTypes];
		if (!m_rgData)
		{
			return CSSMERR_DL_MEMORY_ERROR;
		}
		m_NumRecordTypes = NumRecordTypes;

		return CSSM_OK;
	}

	uint32 neGetNumRecordTypes(void) const
	{
		return m_NumRecordTypes;
	}
};

#endif
