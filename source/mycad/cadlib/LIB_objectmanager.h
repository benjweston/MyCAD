#pragma once

#include <windows.h>
#include <vector>

namespace mycad {
    /**
    * Declaration of ObjectManager class.
    */
	template <typename T>
	class ObjectManager {
	public:
		ObjectManager();
        virtual ~ObjectManager();

		virtual BOOL addObject(T* object);
        virtual BOOL addObject(T* object, HWND hwndFrom);
        virtual BOOL removeObject(const T* object);

        virtual bool getObjectFound(const T* object) const;

        virtual BOOL setActiveObject(T* object);
        virtual T* getActiveObject() const;

        virtual void setObjectInactive(const T* object);

        virtual const std::vector<T*> &getObjects() const;

        virtual void setSize(int size);
        virtual int getSize();

	protected:
		T *m_lpActiveObject;
		std::vector<T*> m_Objects;

	};

    template <typename T>
    ObjectManager<T>::ObjectManager()
        : m_lpActiveObject(0),
        m_Objects(0)
    {}

    template <typename T>
    ObjectManager<T>::~ObjectManager()
    {
        m_Objects.erase(m_Objects.begin(), m_Objects.end());
        m_Objects.clear();

        //delete m_lpActiveObject;
        m_lpActiveObject = nullptr;
    }

    template <typename T>
    BOOL ObjectManager<T>::addObject(T* object)
    {
        BOOL nSuccess = FALSE;
        if (object) {
            if (!getObjectFound(object)) {
                m_Objects.push_back(object);
                nSuccess = TRUE;
            }
        }
        return nSuccess;
    }
    template <typename T>
    BOOL ObjectManager<T>::addObject(T* object, HWND hwndFrom)
    {
        UNREFERENCED_PARAMETER(hwndFrom);
        BOOL nSuccess = FALSE;
        if (object) {
            if (!getObjectFound(object)) {
                m_Objects.push_back(object);
                nSuccess = TRUE;
            }
        }
        return nSuccess;
    }

    template <typename T>
    BOOL ObjectManager<T>::removeObject(const T* object)
    {
        BOOL nSuccess = FALSE;
        if (object) {
            typename std::vector<T*>::iterator result = find(m_Objects.begin(), m_Objects.end(), object);
            if (result != m_Objects.end()) {
                setObjectInactive(object);
                m_Objects.erase(result);

                if (m_Objects.size() == 0) {
                    m_lpActiveObject = nullptr;
                }

                nSuccess = TRUE;
            }
        }
        return nSuccess;
    }

    template <typename T>
    bool ObjectManager<T>::getObjectFound(const T* object) const
    {
        bool bFound = false;

        if (object == nullptr) return bFound;

        typename std::vector<T*>::const_iterator result = find(m_Objects.begin(), m_Objects.end(), object);
        if (result != m_Objects.end()) {
            bFound = true;
        }

        return bFound;
    }

    template <typename T>
    BOOL ObjectManager<T>::setActiveObject(T* object)
    {
        BOOL nSuccess = TRUE;
        if (object != m_lpActiveObject) {
            if (getObjectFound(object)) {
                m_lpActiveObject = object;
            }
            else {
                nSuccess = FALSE;
            }
        }
        return nSuccess;
    }

    template <typename T>
    T* ObjectManager<T>::getActiveObject() const { return m_lpActiveObject; }

    template <typename T>
    void ObjectManager<T>::setObjectInactive(const T* object)
    {
        if (object == m_lpActiveObject) {
            m_lpActiveObject = NULL;
        }
    }

    template <typename T>
    const std::vector<T*>& ObjectManager<T>::getObjects() const { return m_Objects; }

    template <typename T>
    void ObjectManager<T>::setSize(int size) { m_Objects.resize(size); }

    template <typename T>
    int ObjectManager<T>::getSize() { return (int)m_Objects.size(); }
}
