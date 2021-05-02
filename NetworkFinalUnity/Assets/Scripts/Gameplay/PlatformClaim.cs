using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlatformClaim : MonoBehaviour
{
    private ulong pOwner;
    Color playerClaim;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void SetOwner(PlayerScript playerHit) 
    {
        pOwner = playerHit.playerID;
        playerClaim = playerHit.generatedColor;
        var tileRenderer = this.GetComponent<Renderer>();
        tileRenderer.material.SetColor("_Color", playerClaim);
    }

    private void OnCollisionEnter(Collision collision) 
    {
        //use ulong owner to specify when object is owned already in final version
        PlayerScript hit = collision.collider.GetComponent<PlayerScript>();

        if (pOwner == hit.playerID)
        {
            Debug.Log("Owned Collision");
        }
        else 
        {
            SetOwner(hit);
            Debug.Log("Unowned Collidision");
        }
        
    }
}
