using MLAPI;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class PlayerScript : MonoBehaviour
{
    public GameObject player;
    public ulong playerID;
    public Color generatedColor;
    public int playerScore;
    //public Canvas scoreCanvas;
    private GameObject scoreGO;
    public TMP_Text scoreText;
    private bool isOwned;
    //public Color playerColor;
    // Start is called before the first frame update
    void Start()
    {
        playerID = GetComponent<NetworkedPlayerController>().NetworkObjectId;
        isOwned = GetComponent<NetworkedPlayerController>().IsOwner;
        scoreGO = GameObject.FindGameObjectWithTag("Score");
        scoreText = scoreGO.GetComponent<TMP_Text>();
        if (NetworkManager.Singleton.IsServer)
        {
            generatedColor = GeneratePlayerColor();
            NetworkInterface.Instance.BroadcastPlayerColor(playerID, generatedColor);
            var cubeRenderer = player.GetComponent<Renderer>();
            cubeRenderer.material.SetColor("_Color", generatedColor);
        }
    }

    public void UpdateColor() 
    {
        var cubeRenderer = player.GetComponent<Renderer>();
        cubeRenderer.material.SetColor("_Color", generatedColor);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private Color GeneratePlayerColor() 
    {
        //playerColor = new Color(Random.Range(0f, 1f), Random.Range(0f, 1f), Random.Range(0f, 1f));
        //return playerColor;
        Debug.Log("Color Generated");
        return new Color(Random.Range(0f, 1f), Random.Range(0f, 1f), Random.Range(0f, 1f));
    }

    public void UpdateScoreText() 
    {
        if (isOwned) 
        {
            string scoreStr = "Score: " + playerScore.ToString();
            scoreText.text = scoreStr;
        }
    }
}
