using UnityEngine;

public class Singleton<T> : MonoBehaviour where T : MonoBehaviour
{
    private static T instance;

    public static T Ins
    {
        get
        {
            if( instance == null )
            {
                GameObject obj;
                obj = GameObject.Find( typeof( T ).Name );
                if( obj == null )
                {
                    obj = new GameObject( typeof( T ).Name );
                    instance = obj.AddComponent<T>();
                }
                else
                {
                    instance = obj.GetComponent<T>();
                }
            }
            return instance;
        }
    }

    public static bool instanceExists = false;

    public void Awake()
    {
        instanceExists = true;
        DontDestroyOnLoad( gameObject );
    }
}