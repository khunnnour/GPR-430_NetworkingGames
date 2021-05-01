using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlatformClaim : MonoBehaviour
{
    ulong owner;
    private int pOwner;
    Color playerClaim;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void SetOwner() 
    {
        pOwner = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerScript>().playerID;
        playerClaim = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerScript>().generatedColor;
        var tileRenderer = this.GetComponent<Renderer>();
        tileRenderer.material.SetColor("_Color", playerClaim);
    }

    private void OnCollisionEnter(Collision collision) 
    {
        //use ulong owner to specify when object is owned already in final version
        if (pOwner == 1)
        {
            Debug.Log("Owned Collision");
        }
        else 
        {
            SetOwner();
            Debug.Log("Unowned Collidision");
        }
        
    }
}
